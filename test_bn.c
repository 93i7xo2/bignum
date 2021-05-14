#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "bn.h"

#define CLOCK_ID CLOCK_MONOTONIC_RAW
#define ONE_SEC 1000000000.0

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

static void fibonacci(uint64_t n, bn *fib)
{
    if (unlikely(n <= 2)) {
        if (n == 0)
            bn_zero(fib);
        else
            bn_set_u32(fib, 1);
        return;
    }

    bn *a1 = fib; /* Use output param fib as a1 */

    bn_t a0, tmp, a;
    bn_init_u32(a0, 0); /*  a0 = 0 */
    bn_set_u32(a1, 1);  /*  a1 = 1 */
    bn_init(tmp);       /* tmp = 0 */
    bn_init(a);

    /* Start at second-highest bit set. */
    for (uint64_t k = ((uint64_t) 1) << (62 - __builtin_clzll(n)); k; k >>= 1) {
        /* Both ways use two squares, two adds, one multipy and one shift. */
        bn_lshift(a0, 1, a); /* a03 = a0 * 2 */
        bn_add(a, a1, a);    /*   ... + a1 */
        bn_sqr(a1, tmp);     /* tmp = a1^2 */
        bn_sqr(a0, a0);      /* a0 = a0 * a0 */
        bn_add(a0, tmp, a0); /*  ... + a1 * a1 */
        bn_mul(a1, a, a1);   /*  a1 = a1 * a */
        if (k & n) {
            bn_swap(a1, a0);    /*  a1 <-> a0 */
            bn_add(a0, a1, a1); /*  a1 += a0 */
        }
    }
    /* Now a1 (alias of output parameter fib) = F[n] */

    bn_free(a0);
    bn_free(tmp);
    bn_free(a);
}

int main(int argc, char *argv[])
{
    bn_t fib = BN_INITIALIZER;

    if (argc < 2)
        return -1;

    unsigned int n = strtoul(argv[1], NULL, 10);
    if (!n)
        return -2;

    struct timespec start = {0, 0};
    struct timespec end = {0, 0};

    dirty_cache();

    clock_gettime(CLOCK_ID, &start);
    fibonacci(n, fib);
    clock_gettime(CLOCK_ID, &end);
    printf("%lf\n", (double) (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / ONE_SEC);
    bn_free(fib);

    return 0;
}
