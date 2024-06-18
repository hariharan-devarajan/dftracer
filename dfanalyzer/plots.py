import dask
import matplotlib.pyplot as plt
import numpy as np
import dask.dataframe as dd
import pandas as pd
from matplotlib import ticker
from typing import Literal, Tuple

TIME_COLS = ['io_time', 'app_io_time']


class DFAnalyzerPlots(object):

    def __init__(self, events: dd.DataFrame, slope_threshold: int, time_granularity: int) -> None:
        self.events = events
        self.slope_threshold = slope_threshold
        self.time_granularity = time_granularity

    def time_bw_timeline(
        self,
        time_col: Literal['io_time', 'app_io_time'],
        figsize: Tuple[int, int],
        bw_unit: Literal['kb', 'mb', 'gb','tb'] = 'kb',
        line1_label: str = 'I/O Time',
        line2_label: str = 'I/O Bandwidth',
        xlabel: str = 'Timeline (sec)',
        y1label: str = 'Time (sec)',
        y2label: str = 'Bandwidth',
        x_num_ticks: int = 10,
        y_num_ticks: int = 5,
    ):
        size_denom = 1024
        y2label_suffix = 'KB/s'
        if bw_unit == 'mb':
            size_denom = 1024 ** 2
            y2label_suffix = 'MB/s'
        elif bw_unit == 'gb':
            size_denom = 1024 ** 3
            y2label_suffix = 'GB/s'
        elif bw_unit == 'tb':
            size_denom = 1024 ** 4
            y2label_suffix = 'TB/s'

        def _set_bw(df: pd.DataFrame):
            for col in TIME_COLS:
                df[f"{col}_bw"] = (df['size'] / size_denom) / (df[col] / 1e6)
            df.replace([np.inf, -np.inf], np.nan, inplace=True)
            df[f"{col}_bw"] = df[f"{col}_bw"].fillna(0)
            return df

        timeline = self._create_timeline(events=self.events) \
                    .reset_index() \
                    .map_partitions(_set_bw) \
                    .compute() \
                    .assign(seconds=self._assign_seconds)

        fig, ax1 = plt.subplots(figsize=figsize)
        if time_col == "io_time":
            phase = 2
        else:
            phase = 3
        timeline.query(f"phase == {phase}").plot.line(ax=ax1, x='seconds', y=time_col,
                           alpha=0.8, color='C0', label=line1_label)

        ax2 = ax1.twinx()
        y2_min = np.min([timeline.query(f"phase == {phase}")[f"{time_col}_bw"].min()])
        y2_max = np.max([timeline.query(f"phase == {phase}")[f"{time_col}_bw"].max()])

        has_y2 = False
        if y2_max > 0:
            timeline.query(f"phase == {phase}").plot.line(ax=ax2, x='seconds', y=f"{time_col}_bw", linestyle='dashed',
                               alpha=0.8, color='C1', label=line2_label)
            y2_min = np.min([timeline[f"{time_col}_bw"].min()])
            y2_max = np.max([timeline[f"{time_col}_bw"].max()])
            ax2.set_ylim([0, y2_max])
            has_y2 = True
        y1_min = np.min([timeline[col].min() for col in TIME_COLS])
        y1_max = np.max([timeline[col].max() for col in TIME_COLS])
        ax1.set_ylim([0, y1_max])



        ax1.yaxis.set_major_formatter(ticker.FuncFormatter(
            lambda x, pos: '{:.0f}'.format(x/1e6)))
        ax1.set_xlabel(xlabel)
        ax1.set_ylabel(y1label)

        ax2.yaxis.set_major_formatter(ticker.FuncFormatter(
            lambda x, pos: '{:.1f}'.format(x)))
        if has_y2:
            ax2.set_ylabel(f"{y2label} ({y2label_suffix})")

        ax1.yaxis.set_major_locator(ticker.LinearLocator(y_num_ticks))
        if has_y2:
            ax2.yaxis.set_major_locator(ticker.LinearLocator(y_num_ticks))

        handles1, labels1 = ax1.get_legend_handles_labels()
        handles2, labels2 = ax2.get_legend_handles_labels()
        ax1.get_legend().remove()
        if has_y2:
            ax2.get_legend().remove()

        # Combine handles and labels
        handles = handles1 + handles2
        labels = labels1 + labels2

        if has_y2:
            # Create the legend
            ax2.legend(handles, labels)

        ax1.minorticks_on()
        if has_y2:
            ax2.minorticks_on()

        ax1.grid(axis='y', which='major')
        ax1.grid(axis='y', which='minor', alpha=0.3)

        ts_min, te_max = dask.compute(
            self.events['ts'].min(), self.events['te'].max())
        ax1.set_xlim([ts_min / 1e6, te_max / 1e6])
        ax1.xaxis.set_major_formatter(ticker.FormatStrFormatter('%d'))
        ax1.xaxis.set_major_locator(ticker.LinearLocator(x_num_ticks))

        return fig, ax1, ax2

    def xfer_size_timeline(
        self,
        figsize: Tuple[int, int],
        unit: Literal['kb', 'mb', 'gb'] = 'kb',
        xlabel: str = 'Timeline (sec)',
        ylabel: str = 'Transfer Size',
        x_num_ticks: int = 10,
        y_num_ticks: int = 5,
    ):
        xfer_col = 'xfer'

        ylabel_denom = 1024
        ylabel_unit = 'KB'
        if unit == 'mb':
            ylabel_denom = 1024 ** 2
            ylabel_unit = 'MB'
        elif unit == 'gb':
            ylabel_denom = 1024 ** 3
            ylabel_unit = 'GB'

        def _set_xfer_size(df: pd.DataFrame):
            df[xfer_col] = df['size'] / ylabel_denom /df['index']
            return df

        timeline = self._create_timeline(events=self.events) \
            .reset_index() \
            .query("phase == 2") \
            .map_partitions(_set_xfer_size) \
            .compute() \
            .assign(seconds=self._assign_seconds)

        fig, ax = plt.subplots(figsize=figsize)

        timeline.plot.line(ax=ax, x='seconds', y=xfer_col,
                           alpha=0.8, color='C3', label='Avg. Transfer Size')

        ax.set_xlabel(xlabel)
        ax.set_ylabel(f"{ylabel} ({ylabel_unit})")
        _, ylim_max = ax.get_ylim()
        ax.set_ylim([0, ylim_max * 2])
        ax.yaxis.set_major_locator(ticker.LinearLocator(y_num_ticks))
        # ax1.yaxis.set_major_formatter(ticker.FuncFormatter(
        #     lambda x, pos: '{:.0f}'.format(x/1e6)))
        ax.yaxis.set_major_formatter(ticker.FuncFormatter(
            lambda x, pos: '{:.1f}'.format(x)))

        # ax.get_legend().remove()
        ax.minorticks_on()
        ax.grid(axis='y', which='major')
        ax.grid(axis='y', which='minor', alpha=0.3)

        ts_min, te_max = dask.compute(
            self.events['ts'].min(), self.events['te'].max())
        ax.set_xlim([ts_min / 1e6, te_max / 1e6])
        ax.xaxis.set_major_formatter(ticker.FormatStrFormatter('%d'))
        ax.xaxis.set_major_locator(ticker.LinearLocator(x_num_ticks))

        return fig, ax

    def _assign_seconds(self, timeline: pd.DataFrame):
        return timeline['trange'] * (self.time_granularity / 1e6)

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
        events['index'] = events['size']
        timeline = events.groupby(['phase','trange', 'pid', 'tid']) \
            .agg({
                'index': 'count',
                'size': 'sum',
                'io_time': 'sum',
                'app_io_time': 'sum',
            }) \
            .groupby(['phase','trange']) \
            .agg({
                'index': 'sum',
                'size': 'sum',
                'io_time': max,
                'app_io_time': max,
            }).reset_index().set_index("trange", sorted=True)

        return timeline
