/*
 * randgreg.c - generate N random gregs using the word lists from greg.c
 *
 * This file includes greg.c (with GREG_NO_MAIN defined) to reuse the
 * static word arrays. Usage:
 *   ./randgreg        # prints 1 random greg (default)
 *   ./randgreg 10     # prints 10 random gregs
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define GREG_NO_MAIN
#include "greg.c"

static inline uint64_t xorshift64star(uint64_t *state) {
    uint64_t x = *state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *state = x;
    return x * 2685821657736338717ULL;
}

int main(int argc, char **argv) {
    uint64_t count = 1;
    if (argc > 1) {
        char *endptr = NULL;
        errno = 0;
        unsigned long long v = strtoull(argv[1], &endptr, 10);
        if (endptr != argv[1] && errno == 0) count = (uint64_t)v;
    }

    const size_t n_names = sizeof(names) / sizeof(names[0]);
    const size_t n_titleVerbs = sizeof(titleVerbs) / sizeof(titleVerbs[0]);
    const size_t n_titles_base = sizeof(titles_base) / sizeof(titles_base[0]);
    const size_t n_suffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    const size_t n_verbs = sizeof(verbs) / sizeof(verbs[0]);
    const size_t n_adjectives = sizeof(adjectives) / sizeof(adjectives[0]);
    const size_t n_things = sizeof(things) / sizeof(things[0]);

    /* Compute maximum lengths to size buffers once */
    size_t max_name = 0, max_title_verb = 0, max_title_base_no_dot = 0, max_title_prefix_suffix = 0;
    size_t max_suffix = 0, max_verb = 0, max_adj = 0, max_thing = 0;

    for (size_t i = 0; i < n_names; ++i) { size_t L = strlen(names[i]); if (L > max_name) max_name = L; }
    for (size_t i = 0; i < n_titleVerbs; ++i) { size_t L = strlen(titleVerbs[i]); if (L > max_title_verb) max_title_verb = L; }
    for (size_t i = 0; i < n_titles_base; ++i) {
        const char *b = titles_base[i];
        const char *dot = strstr(b, "..");
        if (!dot) { size_t L = strlen(b); if (L > max_title_base_no_dot) max_title_base_no_dot = L; }
        else { size_t prefix = (size_t)(dot - b); size_t suffix = strlen(dot + 2); if (prefix + suffix > max_title_prefix_suffix) max_title_prefix_suffix = prefix + suffix; }
    }
    for (size_t i = 0; i < n_suffixes; ++i) { size_t L = strlen(suffixes[i]); if (L > max_suffix) max_suffix = L; }
    for (size_t i = 0; i < n_verbs; ++i) { size_t L = strlen(verbs[i]); if (L > max_verb) max_verb = L; }
    for (size_t i = 0; i < n_adjectives; ++i) { size_t L = strlen(adjectives[i]); if (L > max_adj) max_adj = L; }
    for (size_t i = 0; i < n_things; ++i) { size_t L = strlen(things[i]); if (L > max_thing) max_thing = L; }

    size_t max_title = max_title_base_no_dot;
    size_t alt = max_title_prefix_suffix + max_title_verb;
    if (alt > max_title) max_title = alt;

    size_t line_buf_size = max_name + 1 + max_title + 1 + max_suffix + 1 + max_verb + 1 + max_adj + 1 + max_thing + 2;
    char *linebuf = (char*)malloc(line_buf_size);
    if (!linebuf) { fprintf(stderr, "allocation failed\n"); return 1; }

    const size_t OUTBUF_SIZE = 4 * 1024 * 1024;
    char *outbuf = (char*)malloc(OUTBUF_SIZE);
    if (!outbuf) { fprintf(stderr, "allocation failed\n"); free(linebuf); return 1; }
    size_t outpos = 0;

    /* Seed RNG */
    uint64_t state = (uint64_t)time(NULL);
    state ^= (uintptr_t)&state;
    for (int i = 0; i < 10; ++i) xorshift64star(&state);

    for (uint64_t it = 0; it < count; ++it) {
        uint64_t r;
        char *p = linebuf;

        /* name */
        r = xorshift64star(&state); size_t ni = (size_t)(r % n_names);
        size_t name_len = strlen(names[ni]); memcpy(p, names[ni], name_len); p += name_len; *p++ = ' ';

        /* title (use titles_base; replace ".." with random titleVerb when present) */
        r = xorshift64star(&state); size_t tbi = (size_t)(r % n_titles_base);
        const char *tb = titles_base[tbi];
        const char *dot = strstr(tb, "..");
        if (!dot) {
            size_t L = strlen(tb); memcpy(p, tb, L); p += L;
        } else {
            size_t prefix = (size_t)(dot - tb);
            size_t suffix = strlen(dot + 2);
            r = xorshift64star(&state); size_t tv = (size_t)(r % n_titleVerbs);
            size_t tvl = strlen(titleVerbs[tv]);
            memcpy(p, tb, prefix); p += prefix;
            memcpy(p, titleVerbs[tv], tvl); p += tvl;
            memcpy(p, dot + 2, suffix); p += suffix;
        }
        *p++ = ' ';

        /* suffix */
        r = xorshift64star(&state); size_t si = (size_t)(r % n_suffixes);
        size_t suffix_len = strlen(suffixes[si]); memcpy(p, suffixes[si], suffix_len); p += suffix_len; *p++ = ' ';

        /* verb */
        r = xorshift64star(&state); size_t vi = (size_t)(r % n_verbs);
        size_t verb_len = strlen(verbs[vi]); memcpy(p, verbs[vi], verb_len); p += verb_len; *p++ = ' ';

        /* adjective */
        r = xorshift64star(&state); size_t ai = (size_t)(r % n_adjectives);
        size_t adj_len = strlen(adjectives[ai]); memcpy(p, adjectives[ai], adj_len); p += adj_len; *p++ = ' ';

        /* thing */
        r = xorshift64star(&state); size_t thi = (size_t)(r % n_things);
        size_t thing_len = strlen(things[thi]); memcpy(p, things[thi], thing_len); p += thing_len; *p++ = '\n';

        size_t L = (size_t)(p - linebuf);
        if (outpos + L > OUTBUF_SIZE) {
            if (fwrite(outbuf, 1, outpos, stdout) != outpos) { fprintf(stderr, "write error\n"); free(linebuf); free(outbuf); return 1; }
            outpos = 0;
        }
        memcpy(outbuf + outpos, linebuf, L);
        outpos += L;
    }

    if (outpos) fwrite(outbuf, 1, outpos, stdout);

    free(linebuf);
    free(outbuf);
    return 0;
}
