#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

#include "list.h"
#include "graph.h"
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
    city = malloc(sizeof(char) * 4);
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

/*
 * Counts the number of unique cities from a file
 * Returns an integer of number of cities
 */
int unique_cities(FILE *in, list *l){
    char line[BUFSIZE];
    int numCities = 0;

    // Read a line at a time from the input file until EOF
    while (fgets(line, BUFSIZE, in) != NULL) {
        
        if (line_is_blank(line) || line_is_comment(line) || line_is_integer(line)) {
            // Ignore blank lines and comment lines.
            continue;
        }
        char *city = substring(line, 1, 3);

        list_pos p = list_first(l);
        bool duplicate = false;
        if(list_is_empty(l)){
            list_insert(l, city, list_end(l));
            numCities++;
            continue;
        }
        while (p != list_end(l)){
            char *inspected_value = list_inspect(l, p);

            if(!strcmp(city, inspected_value)){
                duplicate = true;
            }
            p = list_next(l, p);
        }
        if (!duplicate){
            list_insert(l, city, list_end(l));
            numCities++;
        }
    }
    list_pos q = list_first(l);
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
void add_neighbours(FILE *in, graph *g){
    char line[BUFSIZE];
    // Gets the full column of cities
    while (fgets(line, BUFSIZE, in) != NULL) {
        if (line_is_blank(line) || line_is_comment(line) || line_is_integer(line)) {
            // Ignore blank lines and comment lines.
            continue;
        }
        char *col1 = substring(line, 1, 3);
        node *startNode = graph_find_node(g, col1);
        char *col2 = substring(line, 5, 3);
        node *destNode = graph_find_node(g, col2);
        
        if (startNode != NULL && destNode != NULL)
        {
            graph_insert_edge(g, startNode, destNode);
        }
    }

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

    // Gets the amount of unique cities
    list *cities = list_empty(NULL);
    int numberOfCities = unique_cities(in, cities);

    fclose(in);
    //Try to close input file
    if (fclose(in)){
        fprintf(stderr, "Failed to close %s: %s", map, strerror(errno));
        return -1;
    }
    // Create a graph based on the amount of unique cities
    graph *g = graph_empty(numberOfCities);

    in = fopen(map, "r");
    if (in == NULL){
        fprintf(stderr, "Failed to open %s for reading: %s\n", map, strerror(errno));
        return -1;
    }
    // Add nodes to all nodes
    add_nodes(cities, g);

    // Insert neighbours into the graph
    add_neighbours(in, g);
    printf("Graph is empty: %s\n", graph_is_empty(g) ? "true" : "false");
    printf("Does any neighbours exist: %s\n", graph_has_edges(g) ? "true" : "false");

    char input[10];
    char *answer;
    list_pos p;
    bool invalidOrigin;
    bool invalidDest;
    //Asks some user input
    do{
        printf("Enter origin and destination (quit to exit): ");

        fgets(input, 10, stdin);
        answer = substring(input, 1, 4);
        if (strcmp(answer, "quit")){
            char *origin = substring(input, 1, 3);
            char *dest = substring(input, 5, 3);
            p = list_first(cities);
            invalidOrigin = true;
            invalidDest = true;
            //Search the list of cities to confirm input exists
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
                printf("sök\n");
            } else{
                fprintf(stderr, "Invalid input. Try again\n");
            }

        }

    }while (strcmp(answer, "quit"));


    //Try to close input file
    if (fclose(in)){
        fprintf(stderr, "Failed to close %s: %s", map, strerror(errno));
        return -1;
    }
    return 0;
}