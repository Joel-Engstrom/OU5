#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

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

#define BUFSIZE 300
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
    // Read a line at a time from the input file until EOF
    while (fgets(line, BUFSIZE, in) != NULL) {
        if (line_is_blank(line) || line_is_comment(line)) {
            // Ignore blank lines and comment lines.
            continue;
        }
        printf("%s", line);

    }


    //Try to close input file
    if (fclose(in)){
        fprintf(stderr, "Failed to vlose %s: %s", map, strerror(errno));
        return -1;
    }
    return 0;
}