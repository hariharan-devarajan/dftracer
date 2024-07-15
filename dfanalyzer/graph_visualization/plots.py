# plots for graph visualization

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
import dask.dataframe as dd

class GrepIOPlots:

    def histogram(self,data,x_label):
        # Create the plot with logarithmic y-axis and linear x-axis
        plt.hist(data, bins=23, color='skyblue', edgecolor='black')
        plt.yscale('log')  # Set logarithmic scale for the y-axis
        # Add labels and title
        plt.xlabel(x_label)
        plt.ylabel('Frequency (log scale)')  # Adjust the label for the y-axis
        plt.title('Histogram of '+x_label)
        # Show the plot
        plt.show()

    def bar_graph(self,data,xlabel):
        '''
        Example:
        value_counts = analyzer_mummi1.events['Name'].value_counts().compute()
        bar_graph(value_counts,"Name")
        '''
        # Extract values and counts
        values = data.index.tolist()
        counts = data.values.tolist()
        # Create a bar graph
        plt.figure(figsize=(10, 6))
        plt.bar(values, counts)
        # Add numerical values on the bars
        for i, (xi, yi) in enumerate(zip(values, counts)):
            plt.text(xi, yi, f'{yi:.2f}', ha='center', va='bottom',fontsize=6)
        # Add labels and title
        plt.xlabel('Values')
        plt.ylabel('Counts')
        plt.title("Bar Graph of "+xlabel+" counts")
        # Rotate x-axis labels if needed
        plt.xticks(rotation=45)
        # Show the plot
        plt.show()

    def line_plot(self, ddf):
        '''
        Function: line_graph (Divides the sorted dataframe into 10)
        Usage:
        line_plot(sorted_events)
        '''
        idx = np.linspace(0, len(ddf)-1,10, dtype=int)
        ts = [ddf.compute().iloc[k]['ts'] for k in idx]

        plt.plot(ts,idx, linestyle='--',marker= 'o')
        # Set logarithmic scales for both axes
        # plt.xscale('log')
        # plt.yscale('log')
        # Add labels and title
        plt.xlabel('Time (ts)')
        plt.ylabel('Index in Dataframe')
        plt.title('Index vs Time')
        plt.show()

    def hmap(self, mp=None, df=None, size_bins=[], size_labels=[], degree_bins=[], degree_labels = []):
        # df= inter.compute()
        if mp:
            df= df[df['mount_point']==mp]
        else:
            mp = "All Mount Points"
        # size_bins, size_labels, degree_bins, degree_labels = binned_category()
        df['size_category'] = pd.cut(df['size'], bins=size_bins, labels=size_labels, right=False)
        df['deg_category'] = pd.cut(df['deg_caller'], bins=degree_bins, labels=degree_labels, right=False)

        df['interference'] = df['interference'].astype(float)

        all_combinations = pd.MultiIndex.from_product([df['size_category'].unique(), df['deg_category'].unique()], names=['size_category', 'deg_category'])
        merged = pd.merge(df, pd.DataFrame(index=all_combinations).reset_index(), on=['size_category', 'deg_category'], how='left')
        grouped = merged.groupby(['size_category', 'deg_category']).agg({'interference': 'mean'}).reset_index()

        pivot_table = grouped.pivot(index='size_category', columns='deg_category', values='interference').fillna(-0.5)
        # Create heatmap using seaborn
        plt.figure(figsize=(8, 6))
        pivot_table = 1/pivot_table
        sns.heatmap(pivot_table, annot=True, cmap='YlGnBu', fmt='.1f', linewidths=.5)
        plt.title(f'Average Interference Heatmap {mp}')
        plt.xlabel('Degrees')
        plt.ylabel('Sizes')
        plt.show()
    
    def correlation(self, ddf, group_col = '', col_a = '', col_b = '' ):
        grouped = ddf.groupby(group_col).apply(
        lambda x: x[col_a].corr(x[col_b]),
            meta=('correlation', 'f8')
        ).compute()
        # grouped = ddf.groupby('trange').apply(
        # lambda x: x['dur'].corr(x['interference']),
        #     meta=('correlation', 'f8')
        # ).compute()
        # 1. Binning the trange values
        bin_size = 50  # Adjust this value as needed
        grouped_binned = grouped.groupby(grouped.index // bin_size).mean()
        plt.figure(figsize=(15, 6))
        plt.plot(grouped_binned.index * bin_size, grouped_binned.values, marker='o')
        plt.title('Average Correlation ({col_a} and {col_b})')
        plt.xlabel(f'{group_col} (binned)')
        plt.ylabel('Correlation')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.show()

    def correlation_plot(self, grouped, bin_size = 50, group_col = '', col_a = '', col_b = '' ):
        # grouped = ddf.groupby(group_col).apply(
        # lambda x: x[col_a].corr(x[col_b]),
        #     meta=('correlation', 'f8')
        # ).compute()
        # grouped = ddf.groupby('trange').apply(
        # lambda x: x['dur'].corr(x['interference']),
        #     meta=('correlation', 'f8')
        # ).compute()
        # 1. Binning the trange values
        # bin_size = 50  # Adjust this value as needed
        grouped_binned = grouped.groupby(grouped.index // bin_size).mean()
        plt.figure(figsize=(15, 6))
        plt.plot(grouped_binned.index * bin_size, grouped_binned.values, marker='o')
        plt.title(f'Average Correlation ({col_a} and {col_b})')
        plt.xlabel(f'{group_col} (binned)')
        plt.ylabel('Correlation')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.show()