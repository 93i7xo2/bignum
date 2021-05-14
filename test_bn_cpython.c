#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "bn_cpython.h"

#define CLOCK_ID CLOCK_MONOTONIC_RAW
#define ONE_SEC 1000000000.0

#define unlikely(x) __builtin_expect(!!(x), 0)

void dirty_cache()
{
    /* L3 cache size = 3MB */
    size_t cache_size = 3 * (1 << 20);
    char *a = (char *) malloc(cache_size);
    for (size_t i = 0; i < cache_size; ++i) {
        a[i] = '\0';
    }
    free(a);
}

bn *fibonacci(uint64_t n)
{
    if (unlikely(n <= 2)) {
        if (n == 0)
            return bn_new_from_digit(0);
        return bn_new_from_digit(1);
    }

    bn *a0 = bn_new_from_digit(0); /*  a0 = 0 */
    bn *a1 = bn_new_from_digit(1); /*  a1 = 1 */
    bn *const2 = bn_new_from_digit(2);

    /* Start at second-highest bit set. */
    for (uint64_t k = ((uint64_t) 1) << (62 - __builtin_clzll(n)); k; k >>= 1) {
        /* Both ways use two squares, two adds, one multipy and one shift. */
        bn *t1, *t2, *t3, *tmp1, *tmp2;
        tmp1 = bn_mul(a0, const2);
        t1 = bn_add(tmp1, a1);
        Bn_DECREF(tmp1);
        t2 = bn_mul(a0, a0);
        t3 = bn_mul(a1, a1);
        tmp1 = a0, tmp2 = a1;
        a1 = bn_mul(a1, t1);
        a0 = bn_add(t2, t3);
        Bn_DECREF(t1);
        Bn_DECREF(t2);
        Bn_DECREF(t3);
        Bn_DECREF(tmp1);
        Bn_DECREF(tmp2);
        if (k & n) {
            /*  a1 <-> a0 */
            tmp1 = a1;
            a1 = a0;
            a0 = tmp1;
            /*  a1 += a0 */
            tmp1 = a1;
            a1 = bn_add(a0, a1);
            Bn_DECREF(tmp1);
        }
    }
    /* Now a1 (alias of output parameter fib) = F[n] */
    Bn_DECREF(a0);
    Bn_DECREF(const2);
    return a1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        return -1;

    unsigned int n = strtoul(argv[1], NULL, 10);
    if (!n)
        return -2;

    struct timespec start = {0, 0};
    struct timespec end = {0, 0};

    dirty_cache();

    clock_gettime(CLOCK_ID, &start);
    bn *fib = fibonacci(n);
    clock_gettime(CLOCK_ID, &end);
    printf("%lf\n", (double) (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / ONE_SEC);
    /* bn_print_dec(fib); */

    Bn_DECREF(fib);
}
