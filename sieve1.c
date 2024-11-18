#ifndef __SIEVE1_C__
#define __SIEVE1_C__

#include "include.h"

void sieve1(unsigned long long *global_count, unsigned long long n, int pnum, int pid)
{
    // 计算当前进程处理的数值范围
    unsigned long long low_value=3+pid*(n-1)/pnum;//the smallest value handled by this process
    unsigned long long high_value=2+(pid+1)*(n-1)/pnum;//the largest value handled by this process
    low_value = low_value + (low_value + 1) % 2;  // 保证low_value为奇数
    high_value = high_value - (high_value + 1) % 2;  // 保证high_value为奇数

    // if (pid == pnum-1) high_value = n;
    unsigned long long size = (high_value - low_value) / 2 + 1;  // 只存储奇数，所以大小减半

    // 检查进程数是否合适
    if (1+(n-1)/pnum<(int)sqrt((double)n))//high_value of process 0 should be larger than floor(sqrt(n))
    {
        if (pid==0)
            printf("Error: Too many processes.\n");
        MPI_Finalize();
        exit(0);
    }

    // 分配标记数组
    char *marked = (char*)malloc(size);
    if (marked == NULL)
    {
        printf("Error: Cannot allocate enough memory.\n");
        MPI_Finalize();
        exit(0);
    }
    memset(marked, 0, size);

    unsigned long long prime = 3;  // 从3开始，因为2已经被排除
    unsigned long long index=0;
    do {
        // 计算第一个需要标记的复合数的位置
        unsigned long long first;
        if (prime * prime > low_value)
            first = (prime * prime - low_value) / 2;
        else {
            unsigned long long first_multiple = low_value + ((prime - (low_value % prime)) % prime);
            if (first_multiple % 2 == 0) first_multiple += prime;  // 确保是奇数
            first = (first_multiple - low_value) / 2;
        }

        // 标记所有该素数的奇数倍
        for (unsigned long long i = first; i < size; i += prime)
            marked[i] = 1;
        index++;
        // 在进程0中找下一个素数
        if (pid == 0) {
            unsigned long long i = (prime - 3) / 2 + 1;
            // while (i < size && marked[i]) i++;
            while (marked[index]==1) index++;
            prime = 3 + 2 * index;
        }
        
        // 广播下一个素数
        MPI_Bcast(&prime, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);
    } while (prime * prime <= n);

    // 计算当前进程找到的素数个数
    unsigned long long count = 0;
    for (unsigned long long i = 0; i < size; i++)
        if (marked[i] == 0)
            count++;

    MPI_Reduce(&count,global_count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    if (pid == 0) (*global_count)++;

    free(marked);
}


#endif

