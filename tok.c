/*
 * Copyright (c) 2013-2017 Ilya Kaliman
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "vimol.h"

struct tokq {
	int nelts, nalloc;
	char **data;
};

int
tok_int(tok_t tok)
{
	assert(tok != NULL);

	return ((int)strtol(tok, NULL, 10));
}

double
tok_double(tok_t tok)
{
	assert(tok != NULL);

	return (strtod(tok, NULL));
}

int
tok_bool(tok_t tok)
{
	assert(tok != NULL);

	return (strcasecmp(tok, "true") == 0);
}

const char *
tok_string(tok_t tok)
{
	assert(tok != NULL);

	return ((const char *)tok);
}

color_t
tok_color(tok_t tok)
{
	int r, g, b;

	assert(tok != NULL);

	r = g = b = 0;
	sscanf(tok, "[ %d %d %d ]", &r, &g, &b);

	return (color_rgb(r, g, b));
}

vec_t
tok_vec(tok_t tok)
{
	double x, y, z;

	assert(tok != NULL);

	x = y = z = 0.0;
	sscanf(tok, "[ %lf %lf %lf ]", &x, &y, &z);

	return (vec_xyz(x, y, z));
}

static void
tokq_push_back(struct tokq *tokq, char *str)
{
	if (tokq->nelts == tokq->nalloc) {
		tokq->nalloc *= 2;
		tokq->data = realloc(tokq->data, tokq->nalloc * sizeof(char *));
	}
	tokq->data[tokq->nelts++] = str;
}

static char *
next_token(const char **str)
{
	size_t size = 0;
	const char *ptr = *str;

	if (**str == ';')
		return (xstrndup((*str)++, 1));

	if (**str == '"') {
		(*str)++;

		while (**str && **str != '"')
			(*str)++, size++;

		if (**str == '"')
			(*str)++;
		else {
			error_set("no matching '\"' found");
			return (NULL);
		}

		return (xstrndup(ptr + 1, size));
	}

	if (**str == '[') {
		(*str)++, size++;

		while (**str && **str != ']')
			(*str)++, size++;

		if (**str == ']')
			(*str)++, size++;
		else {
			error_set("no matching ']' found");
			return (NULL);
		}

		return (xstrndup(ptr, size));
	}

	while (**str && !isspace(**str) && !strchr(";\"[", **str))
		(*str)++, size++;

	return (xstrndup(ptr, size));
}

static int
parse_string(struct tokq *tokq, const char *str)
{
	char *tok;

	while (*str && isspace(*str))
		str++;

	while (*str) {
		if ((tok = next_token(&str)) == NULL)
			return (FALSE);

		tokq_push_back(tokq, tok);

		while (*str && isspace(*str))
			str++;
	}

	return (TRUE);
}

static struct tokq *
tokq_create_empty(void)
{
	struct tokq *tokq;

	tokq = calloc(1, sizeof(*tokq));
	tokq->nalloc = 8;
	tokq->data = calloc(tokq->nalloc, sizeof(char *));

	return (tokq);
}

struct tokq *
tokq_create(const char *str)
{
	struct tokq *tokq = tokq_create_empty();

	if (!parse_string(tokq, str)) {
		tokq_free(tokq);
		return (NULL);
	}

	return (tokq);
}

struct tokq *
tokq_copy(struct tokq *tokq, int start, int count)
{
	struct tokq *copy;
	int i;

	assert(start >= 0 && start + count <= tokq_count(tokq));

	copy = tokq_create_empty();

	for (i = start; i < start + count; i++)
		tokq_push_back(copy, xstrdup(tokq_tok(tokq, i)));

	return (copy);
}

void
tokq_free(struct tokq *tokq)
{
	int i;

	for (i = 0; i < tokq_count(tokq); i++)
		free(tokq->data[i]);

	free(tokq->data);
	free(tokq);
}

char *
tokq_strcat(struct tokq *tokq, int start, int count)
{
	char *s;
	int i;

	assert(start >= 0 && start + count <= tokq_count(tokq));

	s = xstrdup("");

	for (i = start; i < start + count; i++) {
		s = xstrcat(s, tokq->data[i]);

		if (i < start + count - 1)
			s = xstrcat(s, " ");
	}

	return (s);
}

int
tokq_count(struct tokq *tokq)
{
	return (tokq->nelts);
}

tok_t
tokq_tok(struct tokq *tokq, int idx)
{
	assert(idx >= 0 && idx < tokq_count(tokq));

	return (tokq->data[idx]);
}
