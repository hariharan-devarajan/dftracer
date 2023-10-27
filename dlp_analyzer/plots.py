import matplotlib.pyplot as plt
import numpy as np
import dask.dataframe as dd
import pandas as pd
from matplotlib import ticker
from typing import Literal, Tuple

DELTA_BINS = [
    0,
    0.001,
    0.01,
    0.1,
    0.25,
    0.5,
    0.75,
    0.9
]
XFER_SIZE_BINS = [
    -np.inf,
    4 * 1024.0,
    16 * 1024.0,
    64 * 1024.0,
    256 * 1024.0,
    1 * 1024.0 * 1024.0,
    4 * 1024.0 * 1024.0,
    16 * 1024.0 * 1024.0,
    64 * 1024.0 * 1024.0,
    np.inf
]
XFER_SIZE_BIN_NAMES = [
    '<4 KB',
    '4 KB',
    '16 KB',
    '64 KB',
    '256 KB',
    '1 MB',
    '4 MB',
    '16 MB',
    '64 MB',
    '>64 MB'
]


class DLPAnalyzerPlots(object):

    def __init__(self, events: dd.DataFrame) -> None:
        self.events = events

    def bottleneck_timeline(
        self,
        figsize: Tuple[int, int],
        xlabel: str = 'Timeline (sec)',
        ylabel: str = 'I/O Time',
    ):
        metric = 'dur'
        slope_col = f"{metric}_slope"

        def _set_slope_and_score(df: pd.DataFrame, metric_max: dd.core.Scalar):
            bin_col, pero_col, per_rev_col, per_rev_cs_col, per_rev_cs_diff_col, score_col, sum_col, th_col = (
                f"{metric}_bin",
                f"{metric}_pero",
                f"{metric}_per_rev",
                f"{metric}_per_rev_cs",
                f"{metric}_per_rev_cs_diff",
                f"{metric}_score",
                f"{metric}_sum",
                f"{metric}_th",
            )

            df[pero_col] = df[metric] / metric_max
            df[bin_col] = np.digitize(df[pero_col], bins=DELTA_BINS, right=True)
            df[th_col] = np.choose(df[bin_col] - 1, choices=DELTA_BINS, mode='clip')

            df['index_sum'] = df['index'].sum()
            df['index_cs'] = df['index'].cumsum()
            df['index_cs_per'] = 1 - df['index_cs'] / df['index_sum']
            df['index_cs_per_rev'] = 1 - df['index_cs_per']
            df['index_cs_per_rev_diff'] = 0.0
            df['index_cs_per_rev_diff'] = df['index_cs_per_rev'].diff().fillna(0)

            df[sum_col] = df[metric].sum()
            df[per_rev_col] = 1 - df[metric] / df[sum_col]
            df[per_rev_cs_col] = (1 - df[per_rev_col]).cumsum()
            df[per_rev_cs_diff_col] = df[per_rev_cs_col].diff().fillna(0)

            df[slope_col] = np.rad2deg(np.arctan2(df['index_cs_per_rev_diff'], df[per_rev_cs_diff_col]))
            df[slope_col] = df[slope_col].fillna(0)

            return df[[metric, pero_col, slope_col, th_col]]

        timeline = self._create_timeline(events=self.events)

        metric_max = timeline[metric].max()

        timeline = timeline \
            .map_partitions(_set_slope_and_score, metric_max=metric_max) \
            .reset_index() \
            .compute()

        problematic = timeline.query(f"{slope_col} > 0 and {slope_col} < 45")

        fig, ax = plt.subplots(figsize=figsize)

        timeline.plot.line(ax=ax, x='trange', y=metric, alpha=0.8, color='C0')

        overlap = problematic[problematic['trange'].isin(timeline['trange'])]
        colors = np.vectorize(self._color_map)(problematic['dur_th'])

        overlap.plot.scatter(ax=ax, x='trange', y=metric, c=colors, s=96)

        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        ax.get_legend().remove()

        return fig, ax

    def bw_timeline(
        self,
        figsize: Tuple[int, int],
        unit: Literal['kb', 'mb', 'gb'],
        xlabel: str = 'Timeline (sec)',
        ylabel: str = 'Agg. Bandwidth',
    ):
        bw_col = 'bw'

        def _set_bw(df: pd.DataFrame):
            df[bw_col] = df['size'] / df['dur']
            return df

        timeline = self._create_timeline(events=self.events) \
            .map_partitions(_set_bw) \
            .reset_index() \
            .compute()

        ax = timeline.plot.line(x='trange', y=bw_col, figsize=figsize)

        ylabel_denom = 1024
        ylabel_unit = 'KB/s'
        if unit == 'mb':
            ylabel_denom = 1024 ** 2
            ylabel_unit = 'MB/s'
        elif unit == 'gb':
            ylabel_denom = 1024 ** 3
            ylabel_unit = 'GB/s'

        ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, pos: '{:.2f}'.format(x/ylabel_denom)))
        ax.set_xlabel(xlabel)
        ax.set_ylabel(f"{ylabel} ({ylabel_unit})")
        ax.get_legend().remove()

        return ax

    def xfer_size_distribution(
        self,
        figsize: Tuple[int, int],
        xlabel: str = 'Transfer Sizes',
        ylabel: str = 'Frequency',
    ):
        xfer_col, xfer_bin_col, xfer_label_col = (
            'xfer',
            'xfer_bin',
            'xfer_label'
        )

        def _set_xfer_size(df: pd.DataFrame):
            df[xfer_col] = df['size'] / df['index']
            df[xfer_bin_col] = np.digitize(df[xfer_col], bins=XFER_SIZE_BINS, right=True)
            df[xfer_label_col] = np.choose(df[xfer_bin_col] - 1, choices=XFER_SIZE_BIN_NAMES, mode='clip')
            return df

        timeline = self._create_timeline(events=self.events) \
            .map_partitions(_set_xfer_size) \
            .reset_index() \
            .compute()

        xfer_labels = timeline \
            .groupby([xfer_bin_col, xfer_label_col]) \
            .agg({'index': 'sum'}) \
            .reset_index() \
            .sort_values(xfer_bin_col)

        ax = xfer_labels.plot.bar(x=xfer_label_col, y='index', figsize=figsize)

        ax.yaxis.set_major_formatter(ticker.StrMethodFormatter('{x:,.0f}'))
        ax.set_xlabel(xlabel)
        ax.set_ylabel(ylabel)
        ax.get_legend().remove()

        return ax

    @staticmethod
    def _color_map(threshold: float):
        if threshold >= 0.9:
            return 'red'
        elif threshold >= 0.75:
            return 'darkorange'
        elif threshold >= 0.5:
            return 'orange'
        elif threshold >= 0.25:
            return 'gold'
        elif threshold >= 0.1:
            return 'yellow'
        elif threshold >= 0.01:
            return 'yellowgreen'
        elif threshold >= 0.001:
            return 'limegreen'
        else:
            return 'green'

    @staticmethod
    def _create_timeline(events: dd.DataFrame):
        events['index'] = 1

        timeline = events.groupby(['trange', 'pid']) \
            .agg({
                'index': 'count',
                'size': 'sum',
                'dur': 'sum',
            }) \
            .groupby(['trange']) \
            .max()

        return timeline
