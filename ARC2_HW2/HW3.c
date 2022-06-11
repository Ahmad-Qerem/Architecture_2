#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

// include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif
#include <smmintrin.h>
#include <immintrin.h>
#define CLOCK_RATE_GHZ 2.26e9

int sum_scalar(int size,int *a,int *b);
int sum_scalar_unrolling(int size,int *a,int *b);
int sum_vector(int size,int *a,int *b);
int sum_vector_unrolling(int size,int *a,int *b);
int sum_AVX(int size,int *a,int *b);
void randomize_array(int size ,int *A,int *B);
void print_array(int size ,int *A,int *B);

static __inline__ uint64_t RDTSC(){
    uint32_t hi, lo;
    __asm__ volatile(
        "rdtsc"
        : "=a"(lo), "=d"(hi));
    return (((uint64_t)hi) << 32) | ((uint64_t)lo);
}
void benchmark(int n, int *a, int *b, int (*computeSum)(int, int *, int *),char *name){
    // warm up cache
    int sum = computeSum(n, a, b);
    // measure
    uint64_t beginCycle = RDTSC();
    sum += computeSum(n, a, b);
    uint64_t cycles = RDTSC() - beginCycle;
    double microseconds = cycles / CLOCK_RATE_GHZ * 1e6;
    // print results
    printf("%s: ", name);
    if (sum == 2 * sum_scalar(n, a, b)){ // sum is calculated twice. Thefirst function should be strictly called(sum_scalar)
            printf("%.2f microseconds\n\n", microseconds);
        }
    else{
        printf("ERROR!\n");
    }
}

int *ArrayA;
int *ArrayB;
void main(int argc, char **argv){
    int N;
    int Result[4]= {0,0,0,0};
    if (argc<2)
        printf(" Error Please Enter N \n");
    else {
        N=atoi(argv[1]);
        if (N<1){
            printf(" Please Enter Number Greater Than 0 ( Array Size Can't Be Less Than 1 ) \n");
        }else {
            //allocate dynamic array
            ArrayA = (int*) malloc(N * sizeof(int));
            ArrayB = (int*) malloc(N * sizeof(int));
            //fill the array
            randomize_array(N,ArrayA,ArrayB);
            //print_array(N,ArrayA,ArrayB);
            Result[0]=sum_scalar(N,ArrayA,ArrayB);
            Result[1]=sum_scalar_unrolling(N,ArrayA,ArrayB);
            Result[2]=sum_vector(N,ArrayA,ArrayB);
            Result[3]=sum_vector_unrolling(N,ArrayA,ArrayB);

            printf("The Summation1 is : %d \n",Result[0] );
            benchmark(N, ArrayA,ArrayB, sum_scalar,"sum_scalar");

            printf("The Summation2 is : %d \n",Result[1] );
            benchmark(N, ArrayA,ArrayB, sum_scalar_unrolling,"sum_scalar_unrolling");

            printf("The Summation3 is : %d \n",Result[2] );
            benchmark(N, ArrayA,ArrayB, sum_vector,"sum_vector");

            printf("The Summation4 is : %d \n",Result[3] );
            benchmark(N, ArrayA,ArrayB, sum_vector_unrolling,"sum_vector_unrolling");
        }
    }
    return 0;
}

void randomize_array(int size ,int *A,int *B){
    srand(time(NULL));
    for (int i =0 ; i < size ; i++ ){
        A[i]=(rand() %(1000 - (0) + 1)) + (0);
        B[i]=(rand() %(1000 - (0) + 1)) + (0);
    }
}
void print_array(int size ,int *A,int *B){
    printf("Array A = [");
    for (int i =0 ; i < size ; i++ ){
        printf(" %d ",A[i]);
        if (i!=(size-1))
            printf(",");
    }
    printf("]\n");
    printf("Array B = [");
    for (int i =0 ; i < size ; i++ ){
        printf(" %d ",B[i]);
        if (i!=(size-1))
            printf(",");
    }
    printf("]\n");
}

int sum_scalar(int size,int *A,int *B){
    int sum=0;
    for (int i=0 ; i< size ; i++){
        sum+=A[i]*B[i];    
    }
    return sum;
}

int sum_scalar_unrolling(int size,int *A,int *B){
    int sum=0;
    int i=0;
    for (; i< size ; i=i+4){
        sum+=A[i]*B[i];
        sum+=A[i+1]*B[i+1];
        sum+=A[i+2]*B[i+2];
        sum+=A[i+3]*B[i+3];    
    }
    for (; i < size; ++i){
        sum+=A[i]*B[i];
    }
    return sum;
}
/*
int sum_vector(int size,int *a,int *b){
    int prod=0;

    return prod;
}

int sum_vector_unrolling(int size,int *a,int *b){
    return 0;
}*/
int sum_vector(int size,int *a,int *b){
    int prod=0;
    union {
        __m128i m128;
        int32_t i32[4];
    } X;
    union {
        __m128i m128;
        int32_t i32[4];
    } Y;
    union {
        __m128i m128;
        int32_t i32[4];
    } Z;

    for(int i=0; i<size; i=i+4) {
        X.m128 = _mm_load_si128((__m128i * ) & a[i]);
        Y.m128 = _mm_load_si128((__m128i * ) & b[i]);
        Z.m128 = _mm_madd_epi16(X.m128,Y.m128);
        prod += Z.i32[0]+Z.i32[1]+Z.i32[2]+Z.i32[3];
    }

    return prod;
}


int sum_AVX(int size, int *a, int *b) {
    int sum=0;
    int i = 0;
    for (; i + 4 <= size; i += 4) {
        // load 256-bit chunks of each array
        __m128i first_values = _mm_loadu_si128((__m128i*) &a[i]);
        __m128i second_values = _mm_loadu_si128((__m128i*) &b[i]);

        // add each pair of 32-bit integers in the 256-bit chunks
        first_values = _mm_madd_epi16(first_values, second_values);

        // store 256-bit chunk to first array
        _mm_storeu_si128((__m128i*) &a[i], first_values);
    }
    // handle left-over
    for (; i < size; ++i) {
        a[i] *= b[i];
    }


    for (int j=0 ; j< size ; j++){
        sum+=a[j];
    }
    return sum;
}

int sum_vector_unrolling(int size,int *a,int *b){
    int prod=0;
    union {
        __m128i m128;
        int32_t i32[4];
    } X;
    union {
        __m128i m128;
        int32_t i32[4];
    } Y;
    union {
        __m128i m128;
        int32_t i32[4];
    } Z;

    for(int i=0; i<size; i=i+16) {
        X.m128 = _mm_load_si128((__m128i * ) & a[i]);

        Y.m128 = _mm_load_si128((__m128i * ) & b[i]);
        Z.m128 = _mm_madd_epi16(X.m128,Y.m128);
        prod += Z.i32[0]+Z.i32[1]+Z.i32[2]+Z.i32[3];

        X.m128 = _mm_load_si128((__m128i * ) & a[i+4]);
        Y.m128 = _mm_load_si128((__m128i * ) & b[i+4]);
        Z.m128 = _mm_madd_epi16(X.m128,Y.m128);
        prod += Z.i32[0]+Z.i32[1]+Z.i32[2]+Z.i32[3];

        X.m128 = _mm_load_si128((__m128i * ) & a[i+8]);
        Y.m128 = _mm_load_si128((__m128i * ) & b[i+8]);
        Z.m128 = _mm_madd_epi16(X.m128,Y.m128);
        prod += Z.i32[0]+Z.i32[1]+Z.i32[2]+Z.i32[3];

        X.m128 = _mm_load_si128((__m128i * ) & a[i+12]);
        Y.m128 = _mm_load_si128((__m128i * ) & b[i+12]);
        Z.m128 = _mm_madd_epi16(X.m128,Y.m128);
        prod += Z.i32[0]+Z.i32[1]+Z.i32[2]+Z.i32[3];

    }
    return prod;
}

/*printf("Vector X = [");
for (int j = 0; j < 4; j++) {
printf(" %d ", X.i32[j]);
if (j != (3))
printf(",");
}
printf("]\n");
printf("Vector Y = [");
for (int j = 0; j < 4; j++) {
printf(" %d ", Y.i32[j]);
if (j != (3))
printf(",");
}
printf("]\n");
printf("Vector Z = [");
for (int j = 0; j < 4; j++) {
printf(" %d ", Z.i32[j]);
if (j != (3))
printf(",");
}

printf("]\n");*/