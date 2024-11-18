#ifndef __SIEVE3_C__
#define __SIEVE3_C__

#include "include.h"

void sieve3(unsigned long long *global_count, unsigned long long n, int pnum, int pid)
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
    unsigned long long block_size = 1048576; // 2^20
    unsigned long long block_low_value = low_value;
    unsigned long long block_high_value = block_low_value + 2 * (block_size - 1);
    if (block_high_value > high_value) block_high_value = high_value;

    do {
        unsigned long long index = 0;
        unsigned long long prime = 3;
        while (prime * prime <= block_high_value) {
            unsigned long long first;
            if (prime * prime > block_low_value)
                first = (prime * prime - block_low_value) / 2;
            else {
                if ((block_low_value % prime) == 0)
                    first = 0;
                else {
                    unsigned long long first_multiple = block_low_value + ((prime - (block_low_value % prime)) % prime);
                    if (first_multiple % 2 == 0) first_multiple += prime;
                    first = (first_multiple - block_low_value) / 2;
                }
            }

            for (unsigned long long i = first + (block_low_value - low_value) / 2; i <= (block_high_value - low_value) / 2; i += prime)
                marked[i] = 1;

            while (local_prime_marks[++index] == 1);
            prime = 3 + 2 * index;
        }
        block_low_value = block_high_value + 2;
        block_high_value = block_low_value + 2 * (block_size - 1);
        if (block_high_value > high_value)
            block_high_value = high_value;
    } while (block_low_value <= high_value);

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