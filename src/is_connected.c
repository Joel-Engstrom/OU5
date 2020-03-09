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


int unique_cities(FILE *in, const char *map, list *l){
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

void add_neighbours(){

}

void add_nodes(list *cities, graph *g){
    list_pos q = list_first(cities);
    while (q != list_end(cities)){
        // printf("%s\n", (char*)list_inspect(cities, q));
        graph_insert_node(g, (char*)list_inspect(cities, q));
        q = list_next(cities, q);
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
    int numberOfCities = unique_cities(in, map, cities);
    
    // Create a graph based on the amount of unique cities
    graph *g = graph_empty(numberOfCities);

    // Insert nodes into the graph
    add_nodes(cities, g);
    printf("Graph is empty: %s\n", graph_is_empty(g) ? "true" : "false");

    

    //Count number of cities - Klar
    //Create graph with number of cities size - Klar
    //Add neighbours to graph - Inte klar
    //Ask user input - Inte klar

    //Try to close input file
    if (fclose(in)){
        fprintf(stderr, "Failed to close %s: %s", map, strerror(errno));
        return -1;
    }
    return 0;
}