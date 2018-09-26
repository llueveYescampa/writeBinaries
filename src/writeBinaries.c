#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "real.h"

#define SIZE 255
int main(int argc, char *argv[]) 
{
    FILE *fp;
    if (argc == 2 ) {
        if((fp = fopen(argv[1], "r+")  )   == NULL) {
            printf("No such file\n");
            exit(1);
        }
    } else {
        printf("Use: %s  filename \n", argv[0]);       
        exit(0);
    } // endif //

    char *binaryFilename;
    binaryFilename = (char *) malloc(SIZE*sizeof(char));

    memset(binaryFilename, '\0', SIZE );

    strcpy(binaryFilename, argv[1]);
    strcat(binaryFilename,"_bin" );

    int n,myVoid,nnz;
    int *rows, *cols;
    real *vals;
    
    if (!fscanf(fp,"%d %d %d", &n, &n, &nnz)) exit(-1);
    rows = (int *) malloc((n+1)*sizeof(int));    
    cols = (int *) malloc(nnz*sizeof(int));    
    vals = (real *) malloc(nnz*sizeof(real));
    
    int rowVal = 0;
    rows[0] = 0;
    
    // creating the row pointer vector     
    for (int i=0; i<nnz; ++i) {
        if (!fscanf(fp,"%d %d %lf", &myVoid, (cols+i), (vals+i))) ;
        if (myVoid != rowVal) {
            rows[myVoid]=i;
            rowVal=myVoid;            
        } // end if //
    } // end for //
    rows[n]=nnz;
    // end of creating the row pointer vector     
    fclose(fp);

    FILE *ptr_myfile;

    // opening binary file for matrix in CSR format //
    ptr_myfile=fopen(binaryFilename,"wb");
    if (ptr_myfile) {
        fwrite(&n,   sizeof(int), 1,    ptr_myfile);
        fwrite(&nnz, sizeof(int), 1,    ptr_myfile);
        fwrite(rows,sizeof(int), (n+1), ptr_myfile);
        fwrite(cols,sizeof(int), (nnz), ptr_myfile);
        fwrite(vals,sizeof(real),(nnz), ptr_myfile);
	} else {    
        printf("Unable to open matrix file!");
        return -1;
    } // end if //
    fclose(ptr_myfile); // closing binary file for matrix in CSR format //
    // end of opening binary file for matrix in CSR format //



    real *x;
    x = (real *) calloc(n,sizeof(real));
    // creating an solution file 
    for (int row=0, k=0; row<n; ++row) {
        int nnzPr = rows[row+1] - rows[row];
        for (int i=0; i<nnzPr; ++i, ++k) {
            x[row] += (cols[k]+1)*vals[k];
        } // end for //
    } // end for //
    free(binaryFilename);
    binaryFilename = (char *) malloc(SIZE*sizeof(char));
    memset(binaryFilename, '\0', SIZE );
    strncpy(binaryFilename, argv[1], (strlen(argv[1])-2));
    strcat(binaryFilename,"out_bin" );
    ptr_myfile=fopen(binaryFilename,"wb"); // opening binary file for input vector //
    if (ptr_myfile) {
        fwrite(x,sizeof(real),(n), ptr_myfile);
	} else {    
        printf("Unable to open file!");
        return -1;
    } // end if //
    fclose(ptr_myfile); // closing binary file for input vector //

    // creating an input file consisting of a 
    // sequence of real numbers from 1.0 to n
    for (int row=0; row<n; ++row) {
        x[row] = (real) (row+1);
    } // end for //

    memset(binaryFilename, '\0', SIZE );
    strncpy(binaryFilename, argv[1], (strlen(argv[1])-2));
    strcat(binaryFilename,"in_bin" );
    
    ptr_myfile=fopen(binaryFilename,"wb"); // opening binary file for input vector //
    if (ptr_myfile) {
        fwrite(x,sizeof(real),(n), ptr_myfile);
	} else {    
        printf("Unable to open file!");
        return -1;
    } // end if //
    fclose(ptr_myfile); // closing binary file for input vector //
    
    free(x);    
    free(rows);
    free(cols);
    free(vals);
    free(binaryFilename);
    return 0;    
} // end main() //
