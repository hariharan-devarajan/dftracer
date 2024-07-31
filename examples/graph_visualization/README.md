# Ipycytoscape visualization for trace graph

## Overview

This example demonstrates how to represent traces(DFanalyzer events) as graphs and visualize them using ipycytoscape within a Jupyter Notebook environment.

## Requirements

To visualize ipycytoscape graphs in DFanalyzer, the following packages are required:

- **networkx**
- **ipycytoscape**
- **ipywidgets**
  
You can install these packages using pip:
```bash
 pip install networkx ipycytoscape ipywidgets
```

## Graph creation and styling
We use folloing methods to convert the traces into graph and perform visualization.

- **create_nx_graph**:  
    This method from *GraphFunctions* class takes dask dataframe (analyzer.events) and return a networkx graph. In this example, we define nodes as each event in dataframe, and edges between two
  events represent the existance of overlapping time between the events. The definition of nodes/edges may be changed within this method for different use cases.

- **visualize_graph**:
    This method takes takes *networkx* graph object and a *CytoGraph* object. Two methods used from *CytoGraph* are
    -  ***get_json*** is used to convert nx graph into json format requried for ipycytoscape visualization.
    -  ***get_style*** is used for styling the cytoscape visualization. We can modify this method to insert different filters (different coloring and layouts) during the visualization.



## Usage

In this example, we used following trace event to represent as graph and visualize using ipycytoscape. Two different colors represent events related with two mount points.

<img width="596" alt="data" src="https://github.com/helloaashish/LongestCommonSubSequence/assets/43627772/af20f604-c9fa-4280-ad27-fca80af9f821">

The trace data is visualized using ipycytoscape, representing each event as a node, with connections indicating overlapping windows.
Nodes with a degree greater than 2 are styled in red; others in purple.

<img width="666" alt="vis" src="https://github.com/helloaashish/LongestCommonSubSequence/assets/43627772/43d7723e-238a-4df9-b327-e8260e0d3631">

## Additional Resource

- <a href="https://networkx.org/" target="_blank">Networkx</a> 
- <a href="https://ipycytoscape.readthedocs.io/en/master/index.html" target="_blank">Ipycytoscape</a>

