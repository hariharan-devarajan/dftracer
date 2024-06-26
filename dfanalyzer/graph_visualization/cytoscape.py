import json
import networkx as nx
import ipycytoscape
import ipywidgets as widgets
from IPython.display import display
import os
# import ipycytoscape
# import networkx as nx


class CytoGraph:
    
    def get_json(self, graph, degree_dict = {}):
        '''
        Input: networkx graph,  dict (key = node_id and value = degree)
        Returns: json_data (format needed for ipycytoscape visualization)
        '''
        # for pid_tid -> file bipartite graphs
        json_data = {
                    'nodes': [{'data': {'id': str(node[0]), 'degree': int(degree_dict[node[0]])}} for node in graph.nodes(data=True)],
                    'edges': [{'data': {'source': str(edge[0]), 'target': str(edge[1]), 'directed': True}} for edge in graph.edges(data=True)]}

        return json_data
    
    
    def get_rich_json(self, graph, degree_dict = {}):
        '''
        Get json with Nodes and Edges attributes
        Input: networkx graph,  dict (key = node_id and value = degree)
        Returns: json_data (format needed for ipycytoscape visualization)
        '''
        # for pid_tid -> file bipartite graphs
        # json_data = {
        #     'nodes': [{'data': {'id': str(node[0]),'degree': int(degree_dict[str(node[0])]), 'bipartite': int(node[1]['bipartite'])}} for node in graph.nodes(data=True)],
        #     'edges': [{'data': {'source': str(edge[0]), 'target': str(edge[1]), 'weight': int(edge[2]['weight'])}} for edge in graph.edges(data=True)]}

        # # for unweighted unipartite graphs
        # json_data = {
        #     'nodes': [{'data': {'id': str(node[0]), 'degree': int(degree_dict[node[0]])}} for node in graph.nodes(data=True)],
        #     'edges': [{'data': {'source': str(edge[0]), 'target': str(edge[1]), 'directed': True}} for edge in graph.edges(data=True)]}

        # for directed
        # Prepare nodes and edges data
        nodes = [{'data': {'id': str(node), 'label': str(node), 'outdegree': int(graph.out_degree(node))}} for node in graph.nodes()]
        edges = []
        for edge in graph.edges(data=True):
            u , v, t = edge[0], edge[1], edge[2]['wt']
            edge_data = {'data': {'source': str(u), 'target': str(v), 'wt': int(t)}}
            if graph.has_edge(v, u):
                edge_data['classes'] = 'bidirectional'
            edges.append(edge_data)
        
        json_data = {'nodes': nodes, 'edges': edges}

        return json_data

    def get_style(self, style_name):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        styles_file = os.path.join(current_dir, 'cystyles.json')
        with open(styles_file, 'r') as f:
            styles = json.load(f)
        return styles.get(style_name, [])
    
    def temporal_view(self,df):
        '''
        Create ipywidget visualization for visualizing prev and next timestamp graphs using trange as tiemstamp
        '''
        def get_graphs(df, each):
            g_data = df.query(f'trange <= {each}')
            G = nx.from_pandas_edgelist(g_data, source='src', target='dest', edge_attr= 'wt', create_using=nx.DiGraph())
            json_data = self.get_rich_json(G)
            return json_data
        
        views_t = sorted(df.trange.unique())
        views = [get_graphs(df,each) for each in views_t]

        # Initialize ipycytoscape graph widgets with the first views
        graph1 = ipycytoscape.CytoscapeWidget()
        graph1.graph.add_graph_from_json(views[0])
        graph1.set_layout(name = 'dagre')
        graph1.set_style(self.get_style('direct'))

        graph2 = ipycytoscape.CytoscapeWidget()
        graph2.graph.add_graph_from_json(views[1])
        graph2.set_layout(name = 'dagre')
        graph2.set_style(self.get_style('direct'))

            # Navigation buttons
        button_selected = widgets.Button(description="Prev")
        button_next = widgets.Button(description="Next")
        output = widgets.Output()

        # Index to keep track of current view
        current_view = 0

        def update_graph_views():
            nonlocal current_view
            graph1.graph.clear()
            graph1.graph.add_graph_from_json(views[current_view])
            graph1.set_layout(name = 'dagre')
            graph1.set_style(self.get_style('direct'))
            
            next_view = (current_view + 1) % len(views)
            graph2.graph.clear()
            graph2.graph.add_graph_from_json(views[next_view])
            graph2.set_layout(name = 'dagre')
            graph2.set_style(self.get_style('direct'))
            
            with output:
                output.clear_output()
                print(f"View {current_view + 1} / {len(views)}")

        def on_prev_clicked(b):
            # update_graph_views()
            nonlocal current_view
            current_view = (current_view - 1) % len(views)
            update_graph_views()

        def on_next_clicked(b):
            nonlocal current_view
            current_view = (current_view + 1) % len(views)
            update_graph_views()

        button_selected.on_click(on_prev_clicked)
        button_next.on_click(on_next_clicked)

        # Display widgets side by side
        buttons_box = widgets.HBox([button_selected, button_next])
        graphs_box = widgets.HBox([graph1, graph2])
        display(widgets.VBox([buttons_box, graphs_box, output]))



class GraphFunctions:
    def create_nx_graph(self, events):
        '''
        Input: dataframe to create graph from
        Returns: networkx graph with node as each row in in df, and edge between two nodes exists if there exists any overlapping time.
        '''
        graph = nx.Graph()
        df = events.sort_values('ts').reset_index()
        event_size = len(events)
        for row in range(event_size):
            current_stop_time = df.at[row, 'te']
            for neigh in range(row+1, event_size):
                neigh_start_time = df.at[neigh, 'ts']
                if (neigh_start_time <= current_stop_time):
                    graph.add_edge(df.at[row, 'id'], df.at[neigh, 'id'])
                else:
                    break
        return graph

    def visualize_graph(self, nx_graph, cyto_obj):
        '''
        Input: networkx graph and cytoscape
        Visualize graph using json data
        '''
        app_view = ipycytoscape.CytoscapeWidget()
        degree_dict = dict(nx_graph.degree)
        json_g = cyto_obj.get_json(nx_graph, degree_dict)
        app_view.graph.add_graph_from_json(json_g)
        # graph_view.set_layout(name="circle")
        # app_view.set_layout(name="concentric")
        app_view.set_layout(name="dagre",spacingFactor= 1.5,rankDir= "LR", fit=True)
        app_view.set_style(cyto_obj.get_style('default'))
        return app_view

    def visualize_nxgraph(self, nx_graph, cyto_obj):
        '''
        Visualize from networkx graph
        '''
        app_view = ipycytoscape.CytoscapeWidget()
        app_view.graph.add_graph_from_networkx(nx_graph, directed=True)
        app_view.set_layout(name="dagre")
        app_view.set_style(cyto_obj.get_style('directed'))
        return app_view
