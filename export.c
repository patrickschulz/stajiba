#include "export.h"

#include <stdio.h>

int export_print_to_file(struct vector* x, struct vector* y, const char* filename, const char* header)
{
    if(vector_size(x) != vector_size(y))
    {
        fprintf(stderr, "export.print_to_file: x and y dimensions do not match (%zd vs. %zd)", vector_size(x), vector_size(y));
        return 0;
    }
    FILE* file = fopen(filename, "w");
    if(!file)
    {
        fprintf(stderr, "could not open file '%s' for writing", filename);
        return 0;
    }
    if(header)
    {
        fprintf(file, "%s\n", header);
    }
    for(size_t i = 0; i < vector_size(x); ++i)
    {
        /*
        fprintf(
            file,
            "%g + %gi %g + %gi\n",
            creal(vector_get(x, i)),
            cimag(vector_get(x, i)),
            creal(vector_get(y, i)),
            cimag(vector_get(y, i))
        );
        */
        fprintf(
            file,
            "%g %g\n",
            cabs(vector_get(x, i)),
            cabs(vector_get(y, i))
        );
    }
    fclose(file);
    return 1;
}

