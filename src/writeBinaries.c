#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "real.h"

void meanAndSd(real *mean, real *sd,real *data, int n)
{
    real sum = (real) 0.0; 
    real standardDeviation = (real) 0.0;

    for(int i=0; i<n; ++i) {
        sum += data[i];
    } // end for //

    *mean = sum/n;

    for(int i=0; i<n; ++i) {
        standardDeviation += pow(data[i] - *mean, 2);
    } // end for //
    *sd=sqrt(standardDeviation/n);
    
} // end of meanAndSd //


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
    
    const size_t length = strlen(argv[1])-2;

    char *binaryFilename;
    binaryFilename = (char *) malloc(SIZE*sizeof(char));

    memset(binaryFilename, '\0', SIZE );

    strcpy(binaryFilename, argv[1]);
    strcat(binaryFilename,"_bin" );

    int n,currentRow,nnz;
    int *rows, *cols;
    real *vals;
    
    if (!fscanf(fp,"%d %d %d", &n, &n, &nnz)) exit(-1);
    rows = (int *)  calloc((n+1),sizeof(int));    
    cols = (int *)  malloc(nnz*sizeof(int));    
    vals = (real *) malloc(nnz*sizeof(real));
    
    // creating the row pointer vector     
    int previousRow = 0;
    rows[0] = 0;
    if (!fscanf(fp,"%d %d %lf", &currentRow, (cols), (vals))) exit(-1);
    previousRow=currentRow;
    int nnzPerRow=1;
    for (int i=1; i<nnz; ++i) {
        if (!fscanf(fp,"%d %d %lf", &currentRow, (cols+i), (vals+i))) exit(-1);
        
        if (currentRow == previousRow) {
            ++nnzPerRow;    
        } else {
            rows[previousRow+1]=nnzPerRow;
            nnzPerRow=1;
            previousRow=currentRow;
        } // end if //
    } // end for //
    rows[n]=nnzPerRow;
    
    //for (int row=0; row<=n; ++row) {  // I fixed this line on Fri 11 Sep 2020 09:53:38 AM CDT 
    for (int row=0; row<n; ++row) {
        rows[row+1] += rows[row];
    } // end for //
    
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
        
        real *nnzPR=(real *) malloc(n*sizeof(real));
        real meanNnzPerRow,sd;
        for (int row=0; row<n; ++row) {
            nnzPR[row] = rows[row+1] - rows[row];
        } // end for //
        meanAndSd(&meanNnzPerRow,&sd,nnzPR, n);
        free(nnzPR);
        fwrite(&meanNnzPerRow,sizeof(real),1, ptr_myfile);
        fwrite(&sd,sizeof(real),1, ptr_myfile);
	} else {    
        printf("Unable to open matrix file!");
        return -1;
    } // end if //
    fclose(ptr_myfile); // closing binary file for matrix in CSR format //
    // end of opening binary file for matrix in CSR format //

    real *x;
    x = (real *) calloc(n,sizeof(real));
    // creating an solution file 
    //int acum=0;
    for (int row=0, k=0; row<n; ++row) {
        int nnzPr = rows[row+1] - rows[row];
        //acum +=nnzPr;
        for (int i=0; i<nnzPr; ++i, ++k) {
            x[row] += (cols[k]+1)*vals[k];
            //x[row] += vals[k];
        } // end for //
    } // end for //
    
    free(binaryFilename);
    binaryFilename = (char *) malloc(SIZE*sizeof(char));
    memset(binaryFilename, '\0', SIZE );
    strncpy(binaryFilename, argv[1], length);
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
        //x[row] = 1.0;
    } // end for //

    memset(binaryFilename, '\0', SIZE );
    strncpy(binaryFilename, argv[1], length  );
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
