/*
 * greg.c - High-performance Greg generator
 *
 * Generates combinations similar to the provided Python script. By default
 * writes to greg.txt. To quickly test, pass a numeric limit as the first
 * argument (e.g. ./greg 1000) and it will generate up to that many lines.
 *
 * Build with: make
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <inttypes.h>

/* Static data translated from the Python source */
static const char *names[] = {
    "Yousuuf gaimar",
    "Greg",
    "Gregory",
    "Useef gaimar",
    "Grug",
    "Grugory",
    "Youssef",
    "Lunchzone"
};

static const char *titleVerbs[] = {
    "slayer",
    "destroyer",
    "obliterator",
    "tearer",
    "dismantler",
    "killer",
    "defeater",
    "hero",
};

static const char *titles_base[] = {
    "The .. of gods",
    "The .. of gregs",
    "The .. of the Gregory",
    "The .. of useef gaimar",
    "The chosen one",
    "The greg",
    "The gregger",
    "The .. of The Association"
};

static const char *suffixes[] = {
    "of the Greg",
    "of the Gregory",
    "of the Gregory greg greg",
    "of the Greg Association",
    "The I",
    "The II",
    "The III",
    "The IV",
    "The V",
    "The VI",
    "The VII",
    "The VIII",
    "The IX",
    "The X",
};

static const char *verbs[] = {
    "wrote", "developed", "created", "made", "invented", "did",
    "generated", "compiled", "implemented", "debugged", "programmed",
    "coded", "greg", "gregged", "did", "coped on", "is", "is not",
    "generated", "will always be", "will never be", "planned", "thought of",
    "code reviewed"
};

static const char *adjectives[] = {
    "an amazing", "a cool", "an awesome", "a fantastic", "a great",
    "an incredible", "an outstanding", "a marvelous", "a superb",
    "a brilliant", "an insane", "a fire", "a greg", "a W", "a cool",
    "a real", "a fucking", "a nonexistent", "an inhuman"
};

static const char *things[] = {
    "game", "website", "script", "program", "software", "app", "AI",
    "robot", "machine learning model", "neural network", "greg",
    "greg generator", "programming language", "compiler", "interpreter",
    "chat application", "thing", "greem", "idea", "plugin", "server",
    "website", "webapp", "library"
};

/* Allow other programs to include this file for the word lists by defining
 * GREG_NO_MAIN before including. That prevents compiling the main() here.
 */
#ifndef GREG_NO_MAIN
int main(int argc, char **argv) {
    /* Optional limit for testing */
    uint64_t limit = 0;
    if (argc > 1) {
        char *endptr = NULL;
        errno = 0;
        unsigned long long v = strtoull(argv[1], &endptr, 10);
        if (endptr != argv[1] && errno == 0) limit = (uint64_t)v;
    }

    const size_t n_names = sizeof(names) / sizeof(names[0]);
    const size_t n_titleVerbs = sizeof(titleVerbs) / sizeof(titleVerbs[0]);
    const size_t n_titles_base = sizeof(titles_base) / sizeof(titles_base[0]);
    const size_t n_suffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    const size_t n_verbs = sizeof(verbs) / sizeof(verbs[0]);
    const size_t n_adjectives = sizeof(adjectives) / sizeof(adjectives[0]);
    const size_t n_things = sizeof(things) / sizeof(things[0]);

    /* Expand titles (replace ".." with each titleVerb) */
    size_t n_titles = 0;
    for (size_t i = 0; i < n_titles_base; ++i) {
        if (strstr(titles_base[i], "..") != NULL) n_titles += n_titleVerbs;
        else ++n_titles;
    }

    char **titles = (char **)malloc(n_titles * sizeof(char*));
    size_t *titles_len = (size_t *)malloc(n_titles * sizeof(size_t));
    if (!titles || !titles_len) {
        fprintf(stderr, "allocation failed\n");
        return 1;
    }

    size_t tindex = 0;
    for (size_t i = 0; i < n_titles_base; ++i) {
        char *dot = strstr((char*)titles_base[i], "..");
        if (!dot) {
            size_t bL = strlen(titles_base[i]);
            titles[tindex] = (char*)malloc(bL + 1);
            if (!titles[tindex]) { fprintf(stderr, "allocation failed\n"); return 1; }
            memcpy(titles[tindex], titles_base[i], bL + 1);
            titles_len[tindex] = bL;
            ++tindex;
        } else {
            size_t prefix_len = (size_t)(dot - titles_base[i]);
            size_t suffix_len = strlen(dot + 2);
            for (size_t j = 0; j < n_titleVerbs; ++j) {
                size_t verb_len = strlen(titleVerbs[j]);
                size_t L = prefix_len + verb_len + suffix_len;
                char *s = (char*)malloc(L + 1);
                if (!s) { fprintf(stderr, "allocation failed\n"); return 1; }
                memcpy(s, titles_base[i], prefix_len);
                memcpy(s + prefix_len, titleVerbs[j], verb_len);
                memcpy(s + prefix_len + verb_len, dot + 2, suffix_len);
                s[L] = '\0';
                titles[tindex] = s;
                titles_len[tindex] = L;
                ++tindex;
            }
        }
    }

    /* Precompute lengths */
    size_t *names_len = (size_t*)malloc(n_names * sizeof(size_t));
    size_t *suffixes_len = (size_t*)malloc(n_suffixes * sizeof(size_t));
    size_t *verbs_len = (size_t*)malloc(n_verbs * sizeof(size_t));
    size_t *adjectives_len = (size_t*)malloc(n_adjectives * sizeof(size_t));
    size_t *things_len = (size_t*)malloc(n_things * sizeof(size_t));
    if (!names_len || !suffixes_len || !verbs_len || !adjectives_len || !things_len) {
        fprintf(stderr, "allocation failed\n");
        return 1;
    }

    size_t max_name = 0, max_title = 0, max_suffix = 0, max_verb = 0, max_adj = 0, max_thing = 0;
    for (size_t i = 0; i < n_names; ++i) { names_len[i] = strlen(names[i]); if (names_len[i] > max_name) max_name = names_len[i]; }
    for (size_t i = 0; i < n_titles; ++i) { if (titles_len[i] > max_title) max_title = titles_len[i]; }
    for (size_t i = 0; i < n_suffixes; ++i) { suffixes_len[i] = strlen(suffixes[i]); if (suffixes_len[i] > max_suffix) max_suffix = suffixes_len[i]; }
    for (size_t i = 0; i < n_verbs; ++i) { verbs_len[i] = strlen(verbs[i]); if (verbs_len[i] > max_verb) max_verb = verbs_len[i]; }
    for (size_t i = 0; i < n_adjectives; ++i) { adjectives_len[i] = strlen(adjectives[i]); if (adjectives_len[i] > max_adj) max_adj = adjectives_len[i]; }
    for (size_t i = 0; i < n_things; ++i) { things_len[i] = strlen(things[i]); if (things_len[i] > max_thing) max_thing = things_len[i]; }

    /* Compute totals */
    uint64_t total = (uint64_t)n_names * (uint64_t)n_titles * (uint64_t)n_suffixes * (uint64_t)n_verbs * (uint64_t)n_adjectives * (uint64_t)n_things;

    fprintf(stderr, "Total combinations: %" PRIu64 "\n", total);

    /* Prepare buffers */
    size_t line_buf_size = max_name + 1 + max_title + 1 + max_suffix + 1 + max_verb + 1 + max_adj + 1 + max_thing + 2;
    char *linebuf = (char*)malloc(line_buf_size);
    if (!linebuf) { fprintf(stderr, "allocation failed\n"); return 1; }

    const size_t OUTBUF_SIZE = 16 * 1024 * 1024; /* 16MB buffer */
    char *outbuf = (char*)malloc(OUTBUF_SIZE);
    if (!outbuf) { fprintf(stderr, "allocation failed\n"); return 1; }

    FILE *f = fopen("greg.txt", "wb");
    if (!f) { fprintf(stderr, "failed to open greg.txt: %s\n", strerror(errno)); return 1; }

    uint64_t written_lines = 0;
    size_t outpos = 0;

    clock_t start_clock = clock();

    for (size_t ni = 0; ni < n_names; ++ni) {
        for (size_t ti = 0; ti < n_titles; ++ti) {
            for (size_t si = 0; si < n_suffixes; ++si) {
                for (size_t vi = 0; vi < n_verbs; ++vi) {
                    for (size_t ai = 0; ai < n_adjectives; ++ai) {
                        for (size_t thi = 0; thi < n_things; ++thi) {
                            char *p = linebuf;
                            memcpy(p, names[ni], names_len[ni]); p += names_len[ni]; *p++ = ' ';
                            memcpy(p, titles[ti], titles_len[ti]); p += titles_len[ti]; *p++ = ' ';
                            memcpy(p, suffixes[si], suffixes_len[si]); p += suffixes_len[si]; *p++ = ' ';
                            memcpy(p, verbs[vi], verbs_len[vi]); p += verbs_len[vi]; *p++ = ' ';
                            memcpy(p, adjectives[ai], adjectives_len[ai]); p += adjectives_len[ai]; *p++ = ' ';
                            memcpy(p, things[thi], things_len[thi]); p += things_len[thi]; *p++ = '\n';
                            size_t L = (size_t)(p - linebuf);

                            if (outpos + L > OUTBUF_SIZE) {
                                if (fwrite(outbuf, 1, outpos, f) != outpos) { fprintf(stderr, "write error\n"); fclose(f); return 1; }
                                outpos = 0;
                            }
                            memcpy(outbuf + outpos, linebuf, L);
                            outpos += L;

                            ++written_lines;
                            if (limit && written_lines >= limit) goto done;
                        }
                    }
                }
            }
        }
    }

done:
    if (outpos) {
        if (fwrite(outbuf, 1, outpos, f) != outpos) { fprintf(stderr, "write error\n"); fclose(f); return 1; }
    }

    fclose(f);

    clock_t end_clock = clock();
    double elapsed = (double)(end_clock - start_clock) / (double)CLOCKS_PER_SEC;

    fprintf(stderr, "Wrote %" PRIu64 " lines to greg.txt in %.3fs\n", written_lines, elapsed);

    /* Cleanup (not strictly necessary on exit, but tidy) */
    for (size_t i = 0; i < n_titles; ++i) free(titles[i]);
    free(titles);
    free(titles_len);
    free(names_len);
    free(suffixes_len);
    free(verbs_len);
    free(adjectives_len);
    free(things_len);
    free(linebuf);
    free(outbuf);

    return 0;
}
#endif /* GREG_NO_MAIN */
