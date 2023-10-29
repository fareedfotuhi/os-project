#include "my_alloc.h"

int main(void)
{
    // show_stats();
    // double *arr = (double *)my_malloc(sizeof(double) * 20,(char)80);
    // printf("arr : %p\n",arr);
    // // for (int i = 0; i < 2*sizeof(double); i++)
    // //     printf("arr[%d]:%d \n",i,*((char*)arr + i));
    // show_stats();
    // my_free(arr);
    // show_stats();
    // SetAllocAlgorithm(FIRSTFIT_ALG);

    int *r = (int *)my_malloc(50 * sizeof(int),(char)5);
    int *e = (int *)my_malloc(40 * sizeof(int),(char)2);
    int *q = (int *)my_malloc(20 * sizeof(int),(char)2);
    int *f = (int *)my_malloc(25 * sizeof(int),(char)3);

    // for (int i = 0; i < 2*sizeof(int); i++)
    //     printf("r[%d]:%d \n",i,*((char*)r + i)); 
    // show_stats();
    // my_free(r);
    show_stats();
    my_free(r);
    show_stats();
    int * str = (int *)my_realloc(e, 45*sizeof(int),(char)3);
        for (int i = 0; i < 4*sizeof(int); i++)
        printf("str[%d]:%d \n",i,*((char*)str + i)); 
    show_stats();
    // my_realloc(str, 0,(char)3);
    // show_stats();   

    // for (int i = 0; i < 5*sizeof(char); i++)
    //     printf("str[%d]:%d \n",i,*((char*)r + i)); 
    // strcpy(str, "hossein hossein");
    // show_stats();
    // char *f = (char *)my_realloc(NULL, 50,(char)5);
    // show_stats();

    return 0;
}