#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

// stretchy buffers
#define MAX(a,b) ((a) >= (b) ? (a) : (b))

typedef struct BufHeader {
    size_t len;
    size_t cap;
    char buf[0];
} BufHeader;
// 0000 0001 000000000000

#define buf__hdr(b) ((BufHeader *)((char *)b - offsetof(BufHeader, buf)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n) (buf__fits(b, n) ? 0 : ((b) = buf__grow(b, buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit((b), 1), b[buf_len(b)] = (x), buf__hdr(b)->len++)
#define buf_free(b) ((b) ? free(buf__hdr(b)) : 0)

void *buf__grow(const void *buf, const size_t new_len, const size_t elem_size) {
    const size_t new_cap = MAX(1 + 2*buf_cap(buf), new_len);
    assert(new_cap <= new_cap);
    const size_t new_size = offsetof(BufHeader, buf) + new_cap * elem_size;
    BufHeader *new_hdr;
    if (buf) {
        new_hdr = realloc(buf__hdr(buf), new_size);
    } else {
        new_hdr = malloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}

void buf_test() {
    int *buf = NULL;

    enum { N = 1024};
    for (int i = 0; i < N; ++i) {
        buf_push(buf, i);
    }
    assert(buf_len(buf) == N);

    for (int i = 0; i < buf_len(buf); i++) {
        assert(buf[i] == i);
    }

    buf_free(buf);
}

int main(int args, char **argv) {

    buf_test();

    return 0;
}
