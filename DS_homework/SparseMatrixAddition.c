#include "smatrix.h"
#define True 1
#define False 0

unsigned int smcreate(matrix *a, const unsigned int rows, const unsigned int cols) {
	assert(a != NULL);

	unsigned int capacity = MIN( rows*cols, MAX_ELEMENTS );
	*a = malloc( capacity * sizeof(element));
	(*a)[0].row = rows;
	(*a)[0].col = cols;
	(*a)[0].value = 0; // zero matrix
	return capacity;
}


void smadd(matrix a,matrix b, matrix d){
    assert((a!=NULL)&&(b!=NULL)&&(d!=NULL));
    assert((a[0].row==b[0].row)&&a[0].col==b[0].col); /*Must be same size */

    int a_index=1;
    int b_index=1; /*index for a matrix and b matrix */
    int d_index=1;

    d[0].row=a[0].row;
    d[0].col=b[0].col;
   
    while(True){
            
        /* Check if a_index is greater than a[0].value or b_index than b[0].value */
        if(a_index>a[0].value){
            
            while(True){
                
                if(b_index>b[0].value) d[0].value = --d_index; return d;
                // when both a_index and b_index meets the end, return d after inserting d_index to d[0].value
                d[d_index].row=b[b_index].row;
                d[d_index].col=b[b_index].col;
                d[d_index].value=b[b_index].value;

                b_index++;
                d_index++;                  
            }

        }
        else { /* b_index > b[0].value */

            while(True){
                
                if(a_index>a[0].value) d[0].value = --d_index; return d;

                d[d_index].row=a[a_index].row;
                d[d_index].col=a[a_index].col;
                d[d_index].value=a[a_index].value;

                a_index++;
                d_index++;                  
            }

        } 

        if(a[a_index].row > b[b_index].row){
        
            d[d_index].row=b[b_index].row;
            d[d_index].col=b[b_index].col;
            d[d_index].value=b[b_index].value;

            b_index++;
            d_index++;
        }
        else if(a[a_index].row < b[b_index].row){
            
            d[d_index].row=a[a_index].row;
            d[d_index].col=a[a_index].col;
            d[d_index].value=a[a_index].value;

            a_index++;
            d_index++;
            
        }
        else { /*a[a_index]==b[b_index] */
            if(a[a_index].col > b[b_index].col){
                
                d[d_index].row=b[b_index].row;
                d[d_index].col=b[b_index].col;
                d[d_index].value=b[b_index].value;

                b_index++;
                d_index++;

            }
            else if(a[a_index].col < b[b_index].col){

                d[d_index].row=a[a_index].row;
                d[d_index].col=a[a_index].col;
                d[d_index].value=a[a_index].value;

                a_index++;
                d_index++;
            
            }
            else { /*a[a_index].row,col == b[b_index].row,col */

                d[d_index].row=a[a_index].row;
                d[d_index].col=a[a_index].col;
                d[d_index].value=a[a_index].value + b[b_index].value;

                a_index++;
                b_index++;
                d_index++;
            }

        }
    }

}