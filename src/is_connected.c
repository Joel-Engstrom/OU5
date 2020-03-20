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
/*
 * Read a part of a string, original from integer start with length of integer length
 * Return a substring of original
 */
char *substring(const char *original, int start, int length)
{
    char *city;
    int position = start, c=0;
    // 4 eftersom \0 också behövs på slutet
    city = malloc(sizeof(char) * length+1);
    // Kontrollera så att det finns minne
    if (city == NULL)
    {
        printf("Unable to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    // Körs 3 gånger för att ta dem tre första bokstäverna
    while (c < length) {
        city[c] = original[position+c-1];
        c++;
    }
    city[c] = '\0';
    return city;
}

void read_file(FILE *in, list *l){
    char line[BUFSIZE];
    int numCities = 0;

    // Read a line at a time from the input file until EOF
    while (fgets(line, BUFSIZE, in) != NULL) {
        if (line_is_blank(line) || line_is_comment(line) || line_is_integer(line)) {
            // Ignore blank lines and comment lines.
            continue;
        }
        int whereToCut = has_comment(line) - 1;
        if (whereToCut <= -1)
        {
            whereToCut = strlen(line) - 1;
        }
        //printf("%d\n", whereToCut);
        char *city = substring(line, 1, whereToCut);
        list_insert(l, city, list_end(l));
    }
}

/*
 * Counts the number of unique cities from a file
 * Returns an integer of number of cities
 */
int unique_cities(list *cities, list *edges){
    list_pos P_edges = list_first(edges);
    list_pos P_cities = list_first(cities);
    int numCities = 0;

   
    // Read a line at a time from the input file until EOF
    while (P_edges != list_end(edges)) {
        
        int whereToCut = first_white_space(list_inspect(edges, P_edges));
        int length2 = strlen(list_inspect(edges, P_edges))-whereToCut;
        char *city = substring(list_inspect(edges, P_edges), 1, whereToCut);
        char *city2 = substring(list_inspect(edges, P_edges), whereToCut+2, length2);

        
        bool duplicate = false;

        
        // If the list is empty insert immediatly
        if(list_is_empty(cities)){
            list_insert(cities, city, list_end(cities));
            numCities++;
            duplicate = true;
        }
        // Check if City1 is a duplicate, if not insert it.
        while (P_cities != list_end(cities)){
            char *inspected_value = list_inspect(cities, P_cities);

            if(!strcmp(city, inspected_value)){
                free(city);
                duplicate = true;
            }
            P_cities = list_next(cities, P_cities);
        }
        P_cities = list_first(cities);
        if (!duplicate){
            list_insert(cities, city, list_end(cities));
            numCities++;
        }
        
        // Check if city2 is a duplicate, if not insert it.
        duplicate = false;
        while (P_cities != list_end(cities))
        {
            char *inspected_value = list_inspect(cities, P_cities);

            if(!strcmp(city2, inspected_value)){
                free(city2);
                duplicate = true;
            }
            P_cities = list_next(cities, P_cities);
        }
        P_cities = list_first(cities);
        if (!duplicate){
            list_insert(cities, city2, list_end(cities));
            numCities++;
        }
        P_edges = list_next(edges, P_edges);
    }
    
    return numCities;
}

/*
 * Inserts nodes representing cities in to a graph
 */
void add_nodes(list *cities, graph *g){
    list_pos q = list_first(cities);
    while (q != list_end(cities)){
        graph_insert_node(g, (char*)list_inspect(cities, q));
        q = list_next(cities, q);
    }
}

/*
 * Add the neighbouring cities to each node
 */
void add_neighbours(list *l, graph *g){
    list_pos P_edges = list_first(l);
    
    // Gets the full column of cities
    while (P_edges != list_end(l)) {
        int whereToCut = first_white_space(list_inspect(l, P_edges));
        int length2 = strlen(list_inspect(l, P_edges))-whereToCut;
        char *col1 = substring(list_inspect(l, P_edges), 1, whereToCut);
        char *col2 = substring(list_inspect(l, P_edges), whereToCut+2, strlen(list_inspect(l,P_edges)) - whereToCut);
        //printf("Col1: %s | Col2: %s\n", col1, col2);
 
        node *startNode = graph_find_node(g, col1);
        node *destNode = graph_find_node(g, col2);
        
        if (startNode != NULL && destNode != NULL)
        {
            graph_insert_edge(g, startNode, destNode);
        }
        P_edges = list_next(l, P_edges);
        free(col1);
        free(col2);
    }
}



bool find_path(node *n1, node *n2, graph *g){
    queue *q = queue_empty(NULL);
    graph_node_set_seen(g, n1, true);
    queue_enqueue(q, n1);
    while (!queue_is_empty(q))
    {
        node *p = queue_front(q);
        q = queue_dequeue(q);
        dlist *neighbours = graph_neighbours(g, p);
        dlist_pos pos = dlist_first(neighbours);
        while (!dlist_is_end(neighbours, pos))
        {
            if (!graph_node_is_seen(g, dlist_inspect(neighbours, pos)))
            {
                graph_node_set_seen(g, dlist_inspect(neighbours, pos), true);
                queue_enqueue(q, dlist_inspect(neighbours, pos));
            }
            pos = dlist_next(neighbours, pos);
        }
    }
    queue_kill(q);
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
    char line[BUFSIZE];
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
    list_pos P_edges = list_first(edges);
   

    
    // Gets the full column of cities


    // Gets the amount of unique cities
    list *cities = list_empty(NULL);
    int numberOfCities = unique_cities(cities, edges);

    
    
    // Create a graph based on the amount of unique cities
    graph *g = graph_empty(numberOfCities);

    // Add nodes to all nodes
    add_nodes(cities, g);
    
    
    // Insert neighbours into the graph
    add_neighbours(edges, g);


    char origin[40];
    char dest[40];
    list_pos p;
    bool invalidOrigin;
    bool invalidDest;
    graph_print(g);
    //Asks some user input
    while (1){
        printf("Enter origin and destination (quit to exit): ");

        scanf("%s", origin);
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
                //Kör sökning
                bool hasPath = find_path(graph_find_node(g, origin), graph_find_node(g, dest), g);
                if (hasPath)
                {
                    printf("There is a path from %s to %s.\n\n", origin, dest);
                } else
                {
                    printf("There is no path from %s to %s.\n\n", origin, dest);
                }
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