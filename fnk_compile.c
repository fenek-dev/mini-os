#include <assert.h>
#include <ctype.h>
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

    enum { N = 1024 };
    for (int i = 0; i < N; ++i) {
        buf_push(buf, i);
    }
    assert(buf_len(buf) == N);

    for (int i = 0; i < buf_len(buf); i++) {
        assert(buf[i] == i);
    }

    buf_free(buf);
}

typedef enum TokenKind {
    TOKEN_INT = 128,
    TOKEN_NAME
} TokenKind;

typedef struct Token {
    TokenKind kind;

    union {
        uint64_t val;

        struct {
            const char *start;
            const char *end;
        };
    };
} Token;

char *stream;
Token token;

void next_token() {
    switch (*stream) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            uint64_t val = 0;
            while (isdigit(*stream)) {
                val *= 10;
                val += *stream++ - '0';
            }
            token.kind = TOKEN_INT;
            token.val = val;
            break;
        }


        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_': {
            const char *start = stream;
            while (isalnum(*stream) || *stream == '_') {
                stream++;
            }
            token.start = start;
            token.end = stream;
            token.kind = TOKEN_NAME;

            break;
        }


        default:
            token.kind = *stream++;
            break;
    }
}

void print_token(const Token token) {
    switch (token.kind) {
        case TOKEN_INT:
            printf("INT VALUE: %llu\n", token.val);
            break;
        case TOKEN_NAME:
            printf("NAME VALUE: %.*s\n", (int)(token.end - token.start), token.start);
            break;
        default:
            printf("UNKNOWN VALUE %c\n", token.kind);
            break;
    }
}

void lex_test() {
    char *source = "+()_well1234+HelloWorld_994";
    stream = source;


    next_token();
    while (token.kind) {
        print_token(token);
        next_token();
    }
}


int main(int args, char **argv) {
    buf_test();
    lex_test();

    return 0;
}
