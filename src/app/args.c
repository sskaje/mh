/**
 * src/app/args.c -- parse command line string into argc/argv
 *
 * @author sskaje
 * @license MIT
 * ------------------------------------------------------------------------
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 - , sskaje (https://sskaje.me/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * ------------------------------------------------------------------------
 *
 *
 * cmd line:
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |   | a | b |   | c | d |   | " | e |   | f | " |   | g | \ |   | h |   | \0|
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *       ^
 *     start
 *       p
 *       q => L[0]
 *
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |   | a | b |   | c | d |   | " | e |   | f | " |   | g | \ |   | h |   | \0|
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *               ^   ^
 *                   p
 *              end  q => L[1]
 *                   for a new token, q = p
 *
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |   | a | b |   | c | d |   | " | e |   | f | " |   | g | \ |   | h |   | \0|
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *                               ^
 *                               p quoted = 1
 *                                   ^           ^
 *                                   q => L[2]  end
 *
 *
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |   | a | b |   | c | d |   | " | e |   | f | " |   | g | \ |   | h |   | \0|
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *                                                       ^
 *                                                       p
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 * |   | a | b |   | c | d |   | " | e |   | f | " |   | g |   | h | h |   | \0|
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *                                                       ^           ^
 *                                                       q          end
 *                                          decode escaped '\ ' to ' ' and append
 *                                          append next char 'h' next to ' '
 */

#include "app.h"


struct argv_entry {
    char *ptr;
    int  len;
    STAILQ_ENTRY(argv_entry) next;
};
STAILQ_HEAD(argv_head, argv_entry);

/**
 * Insert a new node to STAILQ
 *
 * @param args
 * @param p
 * @return
 */
static int tailq_new_node(struct argv_head *args, char *p)
{
    struct argv_entry *n = (struct argv_entry *) malloc(sizeof(struct argv_entry));
    n->ptr = p;
    n->len = 0;

    if (STAILQ_EMPTY(args)) {
        STAILQ_INSERT_HEAD(args, n, next);
    } else {
        STAILQ_INSERT_TAIL(args, n, next);
    }
    return 0;
}

/**
 * Append character to last element of STAILQ
 *
 * @param args
 * @param p
 * @return
 */
static int tail_append_char(struct argv_head *args, char p)
{
    struct argv_entry *node = STAILQ_LAST(args, argv_entry, next);
    if (node->ptr[node->len] != p) {
        node->ptr[node->len] = p;
    }
    node->len++;

    return node->len;
}

/**
 * Free STAILQ
 *
 * @param args
 */
static void args_free(struct argv_head * args)
{
    struct argv_entry* np = NULL;
    STAILQ_FOREACH(np, args, next) {
        STAILQ_REMOVE(args, np, argv_entry, next);
        free(np);
    }
    free(args);
}

/**
 * tokenize cmd
 *
 * @param cmd_line
 * @param ntoken
 * @return
 */
static struct argv_head * cmd2list(const char *cmd_line, int* ntoken)
{
    struct argv_head *args = (struct argv_head *) malloc(sizeof(struct argv_head));

    STAILQ_INIT(args);
    STAILQ_EMPTY(args);

    // 跳过前边的空格
    char* s = (char*) cmd_line;
    char* end = (char*) (cmd_line + strlen(cmd_line));
    while (mh_is_space_char(*s) && s < end) {
        s++;
    }

    char* lcmd = strdup(s);
    *ntoken = 0;

    char *p = (char *) lcmd;
    char *q = (char *) lcmd;


#define string_append_char(c) do {          \
            if (!in_token) {                \
                tailq_new_node(args, p);    \
                q = p;                      \
            }                               \
            tail_append_char(args, (c));    \
            ++q;                            \
            in_token = 1;                   \
        } while(0);

#define string_end()                        \
        if (in_token) {                     \
            tail_append_char(args, '\0');   \
            ++q;                            \
            (*ntoken)++;                    \
            in_token = 0;                   \
            q = p;                          \
        }

    char quote  = '\0';
    int  quoted = 0;

    int in_token = 0;

    while (*p) {
        if (*p == '\\') {
            // escape rule
            // \\, \n, \t, \r, \x??, \", \', \0, '\ '


            switch (*(p + 1)) {
                case 'n': // \n
                    string_append_char('\n');
                    break;

                case 'r': // \r
                    string_append_char('\r');
                    break;

                case 't': // \t
                    string_append_char('\t');
                    break;

                case '0': // \0
                    string_append_char('\0');
                    break;

                case 'x': //\x??
                case 'X':
                    if (!mh_is_hex_char(*(p + 2)) || !mh_is_hex_char(*(p + 3))) {
                        // error: invalid hex
                        args_free(args);
                        return NULL;
                    }

                    string_append_char((char) (mh_hex2int(*(p + 2)) << 4 | mh_hex2int(*(p + 3))));
                    p += 2;

                    break;

                case '\'':
                case '\"':
                case ' ':
                    string_append_char(*(p+1));
                    break;

                default:
                    // error: escape character not supported
                    args_free(args);
                    return NULL;
            }

            ++p;
            goto NEXTP;
        } else if (*p == '\"' || *p == '\'') {
            if (!quoted) {
                quoted = 1;
                quote  = *p;
            } else if (quote != *p) {
                string_append_char(*p);
            } else {
                quoted = 0;
                quote  = '\0';
            }
        } else {
            if (quoted) {
                string_append_char(*p);
            } else {
                if (mh_is_space_char(*p)) {
                    if (in_token) {
                        string_end();
                    }
                } else {
                    string_append_char(*p);
                }
            }
        }

        NEXTP:
        ++p;
    }

    string_end();

    return args;
}

/**
 * parse cmd to argc/argv
 *
 * @param cmd
 * @param argc
 * @return
 */
char ** args_parse(const char* cmd, int* argc)
{
    struct argv_head * args = cmd2list(cmd, argc);
    if (args == NULL) {
        *argc = 0;
        return NULL;
    }

    char** argv = malloc((*argc) * sizeof(char *));

    struct argv_entry* np = NULL;
    int i = 0;
    STAILQ_FOREACH(np, args, next) {
        STAILQ_REMOVE(args, np, argv_entry, next);

        argv[i] = np->ptr;
        free(np);
        ++i;
    }
    free(args);

    return argv;
}

// EOF
