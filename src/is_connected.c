#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

#include "list.h"
#include "graph.h"
#include "queue.h"
#define BUFSIZE 300

/*
 * Program that reads a file representing a map and creates a directed graph
 * from the file. Ask the user for some input of two cities/nodes and
 * determines if there is a path between these two cities/nodes.
 * Exits the program when the user types "quit".
 *
 * Authors: Edvin Hedin (c19ehn@cs.umu.se)
 *          Joel Engström  (c19jem@cs.umu.se)
 *
 * Version information:
 *   2020-03-22: v1.0, first public version.
 */

/* Return position of first non-whitespace character or -1 if only
   white-space is found. */
int first_non_white_space(const char *s)
{
    int i = 0; // Start at first char.
    // Advance until we hit EOL as long as we're loooking at white-space.
    while (s[i] && isspace(s[i])) {
        i++;
    }
    if (s[i]) {
        return i; // Return position of found a non-white-space char.
    } else {
        return -1; // Return fail.
    }
}
/**
 * first_white_space() - Reads a string and checks for the first white-space.
 * @s: A string to be searched.
 *
 * Gets a string and returns the position of the first white-space in the string.
 * If no white-space is found, -1 is returned.
 *
 * Returns: The position of the white-space in the string.
 */
int first_white_space(const char *s){
    int i = 0;

    while (s[i] && !isspace(s[i]))
    {
        i++;
    }
    if (s[i])
    {
        return i;
    } else
    {
        return -1;
    }
}

/* Return position of last non-whitespace character or -1 if only
   white-space is found. */
int last_non_white_space(const char *s)
{
    // Start at last char.
    int i = strlen(s) - 1;
    // Move back until we hit beginning-of-line as long as we're
    // loooking at white-space.
    while (i >= 0 && isspace(s[i])) {
        i--;
    }
    if (i >= 0) {
        return i; // Return position of found a non-white-space char.
    } else {
        return -1; // Return fail.
    }
}

/* Return true if s only contains whitespace */
bool line_is_blank(const char *s)
{
    // Line is blank if it only contained white-space chars.
    return first_non_white_space(s) < 0;
}

/* Return true if s is a comment line, i.e. first non-whitespc char is '#' */
bool line_is_comment(const char *s)
{
    int i = first_non_white_space(s);
    return (i >= 0 && s[i] == '#');
}

int has_comment(const char *s){
    int i = 0; // Start at first char.
    // Advance until we hit EOL as long as we're loooking at white-space.
    while (s[i]) {
        if (s[i] == 35) {
            return i; // Return position of found a non-white-space char.
        }
        i++;
    }
    return -1; // Return fail.
}
/* Return true if s is a integer line, i.e. '0'..'9' */
bool line_is_integer(const char *s)
{
    char digit = *s;
    return isdigit(digit);
}
/**
 * substring() - Gets only a part of a string.
 * @original: A string to be substringed.
 * @start: The position to begin substringing from.
 * @length: How long from the start position to cut.
 *
 * Gets a string and returns only a specific part of it.
 *
 * Returns: A specific part of the original string.
 */
char *substring(const char *original, int start, int length)
{
    char *city;
    int position = start, c=0;
    // Allocate memory depending on the length, plus one for '\0'
    city = malloc(sizeof(char) * length+1);
    // Check if the allocation was successful
    if (city == NULL)
    {
        printf("Unable to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    // Copies letter for letter to the new string
    while (c < length) {
        city[c] = original[position+c-1];
        c++;
    }
    // Add end of string character to the end
    city[c] = '\0';
    return city;
}
/**
 * read_file() - Reads a file from the hard drive.
 * @in: A FILE reference.
 * @l: A list to insert each row into.
 *
 * Ignores empty lines, lines beginning with a '#' and numbers.
 *
 * Returns: Nothing but updates the list l.
 */
void read_file(FILE *in, list *l){
    char line[BUFSIZE];

    // Read a line at a time from the input file until EOF
    while (fgets(line, BUFSIZE, in) != NULL) {
        if (line_is_blank(line) || line_is_comment(line) || line_is_integer(line)) {
            // Ignore blank lines, comment lines and integer lines.
            continue;
        }
        // Check for a '#' in the line
        int whereToCut = has_comment(line) - 1;
        // If no '#' was found, use the length of the line instead
        if (whereToCut <= -1)
        {
            whereToCut = strlen(line) - 1;
        }
        // Substring away excess information
        char *city = substring(line, 1, whereToCut);
        list_insert(l, city, list_end(l));
    }
}

/**
 * unique_cities() - Gets the amount of unique cities from a list.
 * @cities: A list of the seperated cities.
 * @edges: A list of unformated cities.
 *
 * Formats the list edges and updates the list cities with this new information.
 * Formating means separating and removing duplicates.
 *
 * Returns: An integer with the number of unique cities.
 */
int unique_cities(list *cities, list *edges){
    list_pos P_edges = list_first(edges);
    list_pos P_cities = list_first(cities);
    int numCities = 0;
   
    // Goes through the full list of edges
    while (P_edges != list_end(edges)) {
        // Checks where to cut for the second substring call
        int whereToCut = first_white_space(list_inspect(edges, P_edges));
        int length2 = strlen(list_inspect(edges, P_edges))-whereToCut;
        // Substrings into two different strings
        char *city = substring(list_inspect(edges, P_edges), 1, whereToCut);
        char *city2 = substring(list_inspect(edges, P_edges), whereToCut+2, length2);
        if (!strcmp(city, "") || !strcmp(city2, ""))
        {
            fprintf(stderr, "ERROR: Bad file format!\n");
            exit(EXIT_FAILURE);
        }
        
        bool duplicate = false;

        // If the list is empty insert immediatly
        if(list_is_empty(cities)){
            // Check if city and city2 are the same, then free city2
            if (!strcmp(city, city2))
            {
                list_insert(cities, city, list_end(cities));
                free(city2);
                numCities++;
                P_edges = list_next(edges, P_edges);
                continue;
            } 
            else // If they aren't the same, insert them both
            {
                list_insert(cities, city, list_end(cities));
                numCities++;
                list_insert(cities, city2, list_end(cities));
                numCities++;
                P_edges = list_next(edges, P_edges);
                continue;
            }  
        }
        P_cities = list_first(cities);
        // Check if City1 is a duplicate, if not insert it
        while (P_cities != list_end(cities)){
            char *inspected_value = list_inspect(cities, P_cities);
            // Check if the two are the same
            if(!strcmp(city, inspected_value)){
                free(city);
                duplicate = true;
                // Break out of the loops since a duplicate was found
                break;
            }
            P_cities = list_next(cities, P_cities);
        }
        P_cities = list_first(cities);
        // If they weren't the same, insert it into the list
        if (!duplicate){
            list_insert(cities, city, list_end(cities));
            numCities++;
        }
        
        // Check if city2 is a duplicate, if not insert it
        duplicate = false;
        while (P_cities != list_end(cities))
        {
            char *inspected_value = list_inspect(cities, P_cities);
            // Check if the two are the same
            if(!strcmp(city2, inspected_value)){
                free(city2);
                duplicate = true;
                // Break out of the loops since a duplicate was found
                break;
            }
            P_cities = list_next(cities, P_cities);
        }
        P_cities = list_first(cities);
        // If they weren't the same, insert it into the list
        if (!duplicate){
            list_insert(cities, city2, list_end(cities));
            numCities++;
        }
        P_edges = list_next(edges, P_edges);
    } 
    return numCities;
}

/**
 * add_nodes() - Adds all nodes into a graph.
 * @cities: A list of cities.
 * @g: Graph to add the nodes to.
 *
 * Updates the graph g with the new nodes added to it.
 *
 * Returns: Nothing but updates the graph.
 */
void add_nodes(list *cities, graph *g){
    list_pos q = list_first(cities);
    while (q != list_end(cities)){
        // Inserts the node into the graph
        graph_insert_node(g, (char*)list_inspect(cities, q));
        q = list_next(cities, q);
    }
}

/**
 * add_neighbours() - Adds neighbouring cities to all nodes.
 * @l: A list of cities.
 * @g: Graph to inspect.
 *
 * Updates all nodes with their respective neighbours.
 *
 * Returns: Nothing but updates the graph.
 */
void add_neighbours(list *l, graph *g){
    list_pos P_edges = list_first(l);
    
    while (P_edges != list_end(l)) {
        // Checks where to substring depending on the length of the first name
        int whereToCut = first_white_space(list_inspect(l, P_edges));
        int length2 = strlen(list_inspect(l, P_edges))-whereToCut;
        // Gets the two cities and seperates them into two strings
        char *col1 = substring(list_inspect(l, P_edges), 1, whereToCut);
        char *col2 = substring(list_inspect(l, P_edges), whereToCut+2, length2);
        // Finds the correct nodes in the graph
        node *startNode = graph_find_node(g, col1);
        node *destNode = graph_find_node(g, col2);
        // If the two correct nodes are found, add them as neighbours
        if (startNode != NULL && destNode != NULL)
        {
            graph_insert_edge(g, startNode, destNode);
        }
        P_edges = list_next(l, P_edges);
        free(col1);
        free(col2);
    }
}
/**
 * find_path() - Check for a path between two nodes
 *               using width-first-method.
 * @n1: First node.
 * @n2: Second node.
 * @g: Graph to inspect.
 *
 * Returns a bool if there is a path between n1 & n2.
 *
 * Returns: True if a path exists between the two nodes.
 */
bool find_path(node *n1, node *n2, graph *g){
    queue *q = queue_empty(NULL);
    //Mark origin node as seen
    graph_node_set_seen(g, n1, true);
    //Add origin node to queue
    queue_enqueue(q, n1);
    while (!queue_is_empty(q))
    {
        node *p = queue_front(q);
        q = queue_dequeue(q);
        //Retrive current node's neighbours
        dlist *neighbours = graph_neighbours(g, p);
        dlist_pos pos = dlist_first(neighbours);
        //Inspect all neighbours
        while (!dlist_is_end(neighbours, pos))
        {
            //Mark the neighbours as seen if not already
            if (!graph_node_is_seen(g, dlist_inspect(neighbours, pos)))
            {
                graph_node_set_seen(g, dlist_inspect(neighbours, pos), true);
                //Add current node's neighbours to queue
                queue_enqueue(q, dlist_inspect(neighbours, pos));
            }
            pos = dlist_next(neighbours, pos);
        }
        dlist_kill(neighbours);
    }
    queue_kill(q);
    //Return true if the destination node was seen
    if (graph_node_is_seen(g, n2))
    {
        return true;
    }
    return false;
}


int main(int argc, const char **argv)
{
    FILE *in;
    const char *map;
    //Verify number of parameters
    if (argc <= 1){
        fprintf(stderr, "Usage: ./isConnected airmap1.map\n");
        return -1;
    }
    //Try to open input file
    map = argv[1];
    in = fopen(map, "r");
    if (in == NULL){
        fprintf(stderr, "Failed to open %s for reading: %s\n", map, strerror(errno));
        return -1;
    }

    // Create lists ..
    list *edges = list_empty(NULL);
    read_file(in, edges);

    // Gets the amount of unique cities
    list *cities = list_empty(NULL);
    int numberOfCities = unique_cities(cities, edges);

    // Create a graph based on the amount of unique cities
    graph *g = graph_empty(numberOfCities);

    // Add nodes to all nodes
    add_nodes(cities, g);
    
    
    // Insert neighbours into the graph
    add_neighbours(edges, g);

    // Create required elements
    char origin[40];
    char dest[40];
    list_pos p;
    bool invalidOrigin;
    bool invalidDest;
    //Asks some user input
    while (1){
        printf("Enter origin and destination (quit to exit): ");
        // Get origin and destination information from the user
        scanf("%s", origin);
        // If the user typed "quit", exit the program
        if (strcmp(origin, "quit")){
            scanf("%s", dest);
            invalidOrigin = true;
            invalidDest = true;

            //Search the list of cities to confirm input exists
            p = list_first(cities);
            for (int i = 0; i < numberOfCities; i++) {
                char *inspected_city = list_inspect(cities, p);
                if (!strcmp(origin, inspected_city)){
                    invalidOrigin = false;
                }
                if (!strcmp(dest, inspected_city)){
                    invalidDest = false;
                }
                p = list_next(cities, p);
            }
            if (!invalidOrigin && !invalidDest){
                // Check if there is a path between the two choosen cities
                bool hasPath = find_path(graph_find_node(g, origin), graph_find_node(g, dest), g);
                if (hasPath)
                {
                    printf("There is a path from %s to %s.\n\n", origin, dest);
                } else
                {
                    printf("There is no path from %s to %s.\n\n", origin, dest);
                }
                // Reset all nodes seen status to false
                graph_reset_seen(g);
            }else{
                fprintf(stderr, "Invalid input. Try again\n\n");
            }
        } else{
            break;
        }
    }
    printf("Normal exit.\n");

    // Cleanup time
    list_pos pos = list_first(cities);
    while (pos != list_end(cities))
    {
        free(list_inspect(cities, pos));
        pos = list_next(cities, pos);
    }
    list_kill(cities);
    
    pos = list_first(edges);
    while (pos != list_end(edges))
    {
        free(list_inspect(edges, pos));
        pos = list_next(edges, pos);
    }
    list_kill(edges);


    graph_kill(g);
    //Try to close input file
    if (fclose(in)){
        fprintf(stderr, "Failed to close %s: %s", map, strerror(errno));
        return -1;
    }
    return 0;
}