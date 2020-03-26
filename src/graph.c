#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "dlist.h"
#include "graph.h"
#include "array_1d.h"

struct graph
{
    array_1d *cities;
    int size;
    int freeIndex;
};

struct node
{
    const char *name;
    int index;
    dlist *neighbours;
    bool seen;
};

// =================== GRAPH STRUCTURE INTERFACE ======================

/**
 * graph_empty() - Create an empty graph.
 * @max_nodes: The maximum number of nodes the graph can hold.
 *
 * Returns: A pointer to the new graph.
 */
graph *graph_empty(int max_nodes){
    graph *g = calloc(1, sizeof(struct graph));
    g->cities = array_1d_create(0, max_nodes, NULL);
    g->freeIndex = 0;
    g->size = max_nodes;
    return g;
}

/**
 * graph_is_empty() - Check if a graph is empty, i.e. has no nodes.
 * @g: Graph to check.
 *
 * Returns: True if graph is empty, otherwise false.
 */
bool graph_is_empty(const graph *g){
    return g->freeIndex==0;
}

/**
 * graph_has_edges() - Check if a graph has any edges.
 * @g: Graph to check.
 *
 * Returns: True if graph has any edges, otherwise false.
 */
bool graph_has_edges(const graph *g){
    for (int i = 0; i < g->size; i++)
    {
        node *inspected = array_1d_inspect_value(g->cities, i);
        if (!dlist_is_empty(inspected->neighbours))
        {
            return true;
        }
    }
    return false;
}

/**
 * graph_insert_node() - Inserts a node with the given name into the graph.
 * @g: Graph to manipulate.
 * @s: Node name.
 *
 * Creates a new node with a copy of the given name and puts it into
 * the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_node(graph *g, const char *s){
    node *n = calloc(1, sizeof(node));
    n->name = s;
    n->neighbours = dlist_empty(NULL);
    n->index = g->freeIndex;
    array_1d_set_value(g->cities, n, g->freeIndex);
    g->freeIndex++;
    return g;
}

/**
 * graph_find_node() - Find a node stored in the graph.
 * @g: Graph to manipulate.
 * @s: Node identifier, e.g. a char *.
 *
 * Returns: A pointer to the found node, or NULL.
 */
node *graph_find_node(const graph *g, const char *s){
    for (int i = 0; i < g->size; i++)
    {
        node *inspected = array_1d_inspect_value(g->cities, i);
        if (!strcmp(inspected->name,s))
        {
            return inspected;
        }
    }
    return NULL;
}

/**
 * graph_node_is_seen() - Return the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to return seen status for.
 *
 * Returns: The seen status for the node.
 */
bool graph_node_is_seen(const graph *g, const node *n){
    return n->seen;
}

/**
 * graph_node_set_seen() - Set the seen status for a node.
 * @g: Graph storing the node.
 * @n: Node in the graph to set seen status for.
 * @s: Status to set.
 *
 * Returns: The modified graph.
 */
graph *graph_node_set_seen(graph *g, node *n, bool seen){
    n->seen = seen;
    return g;
}

/**
 * graph_reset_seen() - Reset the seen status on all nodes in the graph.
 * @g: Graph to modify.
 *
 * Returns: The modified graph.
 */
graph *graph_reset_seen(graph *g){
    for (int i = 0; i < g->size; i++)
    {
        node *inspected = array_1d_inspect_value(g->cities, i);
        inspected->seen = false;
    }
    return g;
}

/**
 * graph_insert_edge() - Insert an edge into the graph.
 * @g: Graph to manipulate.
 * @n1: Source node (pointer) for the edge.
 * @n2: Destination node (pointer) for the edge.
 *
 * NOTE: Undefined unless both nodes are already in the graph.
 *
 * Returns: The modified graph.
 */
graph *graph_insert_edge(graph *g, node *n1, node *n2){
    
    dlist_insert(n1->neighbours, n2, dlist_first(n1->neighbours));
    return g;
}

/**
 * graph_neighbours() - Return a list of neighbour nodes.
 * @g: Graph to inspect.
 * @n: Node to get neighbours for.
 *
 * Returns: A pointer to a list of nodes. Note: The list must be
 * dlist_kill()-ed after use.
 */
dlist *graph_neighbours(const graph *g,const node *n){
    dlist *temp = dlist_empty(NULL);
    node *inspected = array_1d_inspect_value(g->cities, n->index);
    temp = inspected->neighbours;
    return temp;
}

/**
 * graph_kill() - Destroy a given graph.
 * @g: Graph to destroy.
 *
 * Return all dynamic memory used by the graph.
 *
 * Returns: Nothing.
 */
void graph_kill(graph *g){
    for (int i = 0; i < g->size; i++)
    {
        node *inspected = array_1d_inspect_value(g->cities, i);
        dlist_kill(inspected->neighbours);
        free(inspected);
    }
    array_1d_kill(g->cities);
    free(g);
}

/**
 * graph_print() - Iterate over the graph elements and print their values.
 * @g: Graph to inspect.
 *
 * Iterates over the graph and prints its contents.
 *
 * Returns: Nothing.
 */
void graph_print(const graph *g){
    for (int i = 0; i < g->size; i++)
    {
        node *inspected_node = array_1d_inspect_value(g->cities, i);
        printf("Node %d: %s\n", inspected_node->index, inspected_node->name);

    }
    
}
