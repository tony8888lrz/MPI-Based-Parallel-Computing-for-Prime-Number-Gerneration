#ifndef __SIEVE2_C__
#define __SIEVE2_C__

#include "include.h"

void sieve2(unsigned long long *global_count, unsigned long long n, int pnum, int pid)
{
    // 计算当前进程处理的数值范围
    unsigned long long low_value = 3 + pid*(n-1)/pnum;
    unsigned long long high_value = 2 + (pid+1)*(n-1)/pnum;
    low_value = low_value + (low_value + 1) % 2;  // 保证low_value为奇数
    high_value = high_value - (high_value + 1) % 2;  // 保证high_value为奇数
    unsigned long long size = (high_value - low_value) / 2 + 1;  // 只存储奇数

    // 检查进程数是否合适
    if (1+(n-1)/pnum < (int)sqrt((double)n))
    {
        if (pid == 0)
            printf("Error: Too many processes.\n");
        MPI_Finalize();
        exit(0);
    }

    // 分配标记数组
    char *marked = (char*)malloc(size);
    char *local_prime_marks = (char*)malloc((int)sqrt((double)n)); // 用于本地计算素数
    if (marked == NULL || local_prime_marks == NULL)
    {
        printf("Error: Cannot allocate enough memory.\n");
        MPI_Finalize();
        exit(0);
    }
    memset(marked, 0, size);
    memset(local_prime_marks, 0, (int)sqrt((double)n));

    // 本地计算sqrt(n)以内的素数
    unsigned long long limit = (int)sqrt((double)n);
    for (unsigned long long prime = 3; prime * prime <= limit; prime += 2)
    {
        if (!local_prime_marks[(prime-3)/2])
        {
            for (unsigned long long i = prime * prime; i <= limit; i += prime * 2)
            {
                if ((i-3)/2 < (int)sqrt((double)n))
                    local_prime_marks[(i-3)/2] = 1;
            }
        }
    }

    // 对每一个process进行计算筛选
    for (unsigned long long prime = 3; prime * prime <= n; prime += 2)
    {
        if ((prime-3)/2 >= (int)sqrt((double)n) || !local_prime_marks[(prime-3)/2])
        {
            unsigned long long first;
            if (prime * prime > low_value)
                first = (prime * prime - low_value) / 2;
            else {
                unsigned long long first_multiple = low_value + ((prime - (low_value % prime)) % prime);
                if (first_multiple % 2 == 0) first_multiple += prime;
                first = (first_multiple - low_value) / 2;
            }

            for (unsigned long long i = first; i < size; i += prime)
                marked[i] = 1;
        }
    }

    // 计算当前进程找到的素数个数
    unsigned long long count = 0;
    for (unsigned long long i = 0; i < size; i++)
        if (marked[i] == 0)
            count++;

    MPI_Reduce(&count, global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if (pid == 0) (*global_count)++;  // 加上2这个素数

    free(marked);
    free(local_prime_marks);
}

#endif