#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct mappings_entry {
	int cp_uni;
	int n;
	int cp_932[16];
};

struct mappings {
	size_t n;
	size_t nalloc;
	struct mappings_entry *entries;
};

static void mappings_init(struct mappings *map)
{
	map->n = 0;
	map->nalloc = 0;
	map->entries = 0;
}

static void mappings_destroy(struct mappings *map)
{
	if (map->entries)
		free(map->entries);
}

static int mappings_grow(struct mappings *map)
{
	if (map->n >= map->nalloc) {
		struct mappings_entry *new_entries;
		size_t n = map->nalloc << 1, a;
		if (n == 0)
			n = 1;
		else if (n <= map->n)
			return 2;
		a = sizeof(*map->entries) * n;
		if (a / n != sizeof(*map->entries))
			return 2;
		new_entries = realloc(map->entries, a);
		if (!new_entries)
			return 2;
		map->entries = new_entries;
		map->nalloc = n;
	}
	return 0;
}

static int mappings_add(struct mappings *map, int cp_uni, int cp_932)
{
	size_t i;
	size_t s = 0, e = map->n;
	struct mappings_entry *entry;

	for (;;) {
		i = (s + e) / 2;
		entry = &map->entries[i];
		if (e == i || entry->cp_uni > cp_uni) {
			if (e == i) {
				int r = mappings_grow(map);
				if (r)
					return r;
				if (map->n > i) {
					size_t n = map->n - i, a = sizeof(*map->entries) * n;
					if (a / n != sizeof(*map->entries))
						return 2;
					memmove(&map->entries[i + 1], &map->entries[i], a);
				}
				++map->n;
				entry = &map->entries[i];
				entry->cp_uni = cp_uni;
				entry->n = 0;
				break;
			}
			e = i;
		} else if (entry->cp_uni < cp_uni) {
			if (s == i) {
				int r = mappings_grow(map);
				if (r)
					return r;
				if (map->n > i + 1) {
					size_t n = (map->n - i - 1), a = sizeof(*map->entries) * n;
					if (a / n != sizeof(*map->entries))
						return 2;
					memmove(&map->entries[i + 2], &map->entries[i + 1], a);
				}
				++map->n;
				entry = &map->entries[i + 1];
				entry->cp_uni = cp_uni;
				entry->n = 0;
				break;
			}
			s = i;
		} else {
			break;
		}
	}
	if (entry->n >= sizeof(entry->cp_932) / sizeof(*entry->cp_932))
		return 1;
	entry->cp_932[entry->n++] = cp_932;
	return 0;
}

struct generator_entry {
	const char *name;
	const char *prologue;
	const char *epilogue;
	void(*visitor)(const struct mappings_entry *);
};

static int utf32_utf8(char *buf, int k)
{
	int retval = 0;

	if (k < 0x80) {
		buf[0] = k;
		retval = 1;
	} else if (k < 0x800) {
		buf[0] = 0xc0 | (k >> 6);
		buf[1] = 0x80 | (k & 0x3f);
		retval = 2;
	} else if (k < 0x10000) {
		buf[0] = 0xe0 | (k >> 12);
		buf[1] = 0x80 | ((k >> 6) & 0x3f);
		buf[2] = 0x80 | (k & 0x3f);
		retval = 3;
	} else if (k < 0x200000) {
		buf[0] = 0xf0 | (k >> 18);
		buf[1] = 0x80 | ((k >> 12) & 0x3f);
		buf[2] = 0x80 | ((k >> 6) & 0x3f);
		buf[3] = 0x80 | (k & 0x3f);
		retval = 4;
	} else if (k < 0x4000000) {
		buf[0] = 0xf8 | (k >> 24);
		buf[1] = 0x80 | ((k >> 18) & 0x3f);
		buf[2] = 0x80 | ((k >> 12) & 0x3f);
		buf[3] = 0x80 | ((k >> 6) & 0x3f);
		buf[4] = 0x80 | (k & 0x3f);
		retval = 5;
	} else {
		buf[0] = 0xfc | (k >> 30);
		buf[1] = 0x80 | ((k >> 24) & 0x3f);
		buf[2] = 0x80 | ((k >> 18) & 0x3f);
		buf[3] = 0x80 | ((k >> 12) & 0x3f);
		buf[4] = 0x80 | ((k >> 6) & 0x3f);
		buf[5] = 0x80 | (k & 0x3f);
		retval = 6;
	}
	buf[retval] = '\0';

	return retval;
}

static const char epilogue[] =
"close\n";

static const char prologue_to_cp932[] =
"#!/usr/bin/expect -f\n"
"spawn tests/conv_encoding Japanese CP932 UTF-8\n"
"set timeout 1\n"
"\n"
"expect_after {\n"
"    \"\\[^\\r\\n\\]*\\r\\n\" { fail $test }\n"
"}\n";

static const char prologue_to_cp50220[] =
"#!/usr/bin/expect -f\n"
"spawn tests/conv_encoding Japanese CP50220 UTF-8\n"
"set timeout 1\n"
"\n"
"expect_after {\n"
"    \"\\[^\\r\\n\\]*\\r\\n\" { fail $test }\n"
"}\n";

static const char prologue_to_cp50222[] =
"#!/usr/bin/expect -f\n"
"spawn tests/conv_encoding Japanese CP50222 UTF-8\n"
"set timeout 1\n"
"\n"
"expect_after {\n"
"    \"\\[^\\r\\n\\]*\\r\\n\" { fail $test }\n"
"}\n";

static const char prologue_from_cp932[] =
"#!/usr/bin/expect -f\n"
"spawn tests/conv_encoding Japanese UTF-8 CP932\n"
"set timeout 1\n"
"\n"
"expect_after {\n"
"    \"\\[^\\r\\n\\]*\\r\\n\" { fail $test }\n"
"}\n";

static void to_cp932_visitor(const struct mappings_entry *entry)
{
	char buf_uni[32], buf_cp932[8];
	int i;

	if (entry->cp_uni < 32 || entry->cp_uni == 127)
		return;

	i = utf32_utf8(buf_uni, entry->cp_uni);
	buf_uni[i * 4] = '\0';
	while (--i >= 0) {
		unsigned char c = ((unsigned char *)buf_uni)[i];
		buf_uni[i * 4] = '\\';
		buf_uni[i * 4 + 1] = 'x';
		buf_uni[i * 4 + 2] = "0123456789abcdef"[c >> 4];
		buf_uni[i * 4 + 3] = "0123456789abcdef"[c & 15];
	}

	printf("set test \"U+%06X\"\n"
	       "send -- \"%s\r\"\n"
		   "sleep 0.001\n"
	       "expect {\n", entry->cp_uni, buf_uni);

	for (i = 0; i < entry->n; ++i) {
		int len = 0;
		const int c = entry->cp_932[i];
		if (c >= 0x100) {
			len = 2;
			sprintf(buf_cp932, "%%%02x%%%02x", (c >> 8) & 0xff, c & 0xff);
		} else {
			len = 1;
			sprintf(buf_cp932, "%%%02x", c);
		}
		printf("    \"%s (%d)\\r\\n\" { pass $test }\n", buf_cp932, len);
	}

	printf("}\n");
}

static void from_cp932_visitor(const struct mappings_entry *entry)
{
	char buf_uni[32], buf_cp932[8];
	int i, len;

	if (entry->cp_uni < 32 || entry->cp_uni == 127)
		return;

	len = utf32_utf8(buf_uni, entry->cp_uni);
	buf_uni[len * 3] = '\0';
	i = len;
	while (--i >= 0) {
		unsigned char c = ((unsigned char *)buf_uni)[i];
		buf_uni[i * 3] = '%';
		buf_uni[i * 3 + 1] = "0123456789abcdef"[c >> 4];
		buf_uni[i * 3 + 2] = "0123456789abcdef"[c & 15];
	}

	for (i = 0; i < entry->n; ++i) {
		const int c = entry->cp_932[i];
		if (c >= 0x100)
			sprintf(buf_cp932, "\\x%02x\\x%02x", (c >> 8) & 0xff, c & 0xff);
		else
			sprintf(buf_cp932, "\\x%02x", c);
		printf("set test \"U+%06X\"\n"
			   "send -- \"%s\r\"\n"
			   "sleep 0.001\n"
			   "expect {\n"
		       "    \"%s (%d)\\r\\n\" { pass $test }\n"
		       "}\n",
			   entry->cp_uni, buf_cp932, buf_uni, len);
	}
}

static void to_cp50220_visitor(const struct mappings_entry *entry)
{
	char buf_uni[32], buf_cp50220[32];
	int i;

	if (entry->cp_uni < 32 || entry->cp_uni == 127)
		return;

	i = utf32_utf8(buf_uni, entry->cp_uni);
	buf_uni[i * 4] = '\0';
	while (--i >= 0) {
		unsigned char c = ((unsigned char *)buf_uni)[i];
		buf_uni[i * 4] = '\\';
		buf_uni[i * 4 + 1] = 'x';
		buf_uni[i * 4 + 2] = "0123456789abcdef"[c >> 4];
		buf_uni[i * 4 + 3] = "0123456789abcdef"[c & 15];
	}

	printf("set test \"U+%06X\"\n"
	       "send -- \"%s\r\"\n"
		   "sleep 0.001\n"
	       "expect {\n", entry->cp_uni, buf_uni);

	for (i = 0; i < entry->n; ++i) {
		int len = 0;
		const int c = entry->cp_932[i];
		if (c >= 0xa1 && c < 0xe0) {
			static const int jisx0208_tl_map[] = {
				0x0000, 0x2123, 0x2156, 0x2157, 0x2122, 0x2126, 0x2572, 0x2521,
				0x2523, 0x2525, 0x2527, 0x2529, 0x2563, 0x2565, 0x2567, 0x2543,
				0x213c, 0x2522, 0x2524, 0x2526, 0x2528, 0x252a, 0x252b, 0x252d,
				0x252f, 0x2531, 0x2533, 0x2535, 0x2537, 0x2539, 0x253b, 0x253d,
				0x253f, 0x2541, 0x2544, 0x2546, 0x2548, 0x254a, 0x254b, 0x254c,
				0x254d, 0x254e, 0x254f, 0x2552, 0x2555, 0x2558, 0x255b, 0x255e,
				0x255f, 0x2560, 0x2561, 0x2562, 0x2564, 0x2566, 0x2568, 0x2569,
				0x256a, 0x256b, 0x256c, 0x256d, 0x256f, 0x2573, 0x212b, 0x212c
			};
			const int j = jisx0208_tl_map[c - 0xa0];
			len = 8;
			sprintf(buf_cp50220, "%%1b%%24%%42%%%02x%%%02x%%1b%%28%%42", j >> 8, j & 0xff);
		} else if (c >= 0x100) {
			const int j = ((((c & 0xff00) - (c >= 0xe000 ? 0xb000: 0x7000)) << 1) | ((c & 0xff) - (c & 0x80 ? 32: 31))) - ((c & 0xff) >= 159 ? 94: 0x100);
			len = 8;
			sprintf(buf_cp50220, "%%1b%%24%%42%%%02x%%%02x%%1b%%28%%42", j >> 8, j & 0xff);
		} else {
			len = 1;
			sprintf(buf_cp50220, "%%%02x", c);
		}
		printf("    \"%s (%d)\\r\\n\" { pass $test }\n", buf_cp50220, len);
	}

	printf("}\n");
}

static void to_cp50222_visitor(const struct mappings_entry *entry)
{
	char buf_uni[32], buf_cp50220[32];
	int i;

	if (entry->cp_uni < 32 || entry->cp_uni == 127)
		return;

	i = utf32_utf8(buf_uni, entry->cp_uni);
	buf_uni[i * 4] = '\0';
	while (--i >= 0) {
		unsigned char c = ((unsigned char *)buf_uni)[i];
		buf_uni[i * 4] = '\\';
		buf_uni[i * 4 + 1] = 'x';
		buf_uni[i * 4 + 2] = "0123456789abcdef"[c >> 4];
		buf_uni[i * 4 + 3] = "0123456789abcdef"[c & 15];
	}

	printf("set test \"U+%06X\"\n"
	       "send -- \"%s\r\"\n"
		   "sleep 0.001\n"
	       "expect {\n", entry->cp_uni, buf_uni);

	for (i = 0; i < entry->n; ++i) {
		int len = 0;
		const int c = entry->cp_932[i];
		if (c >= 0xa1 && c < 0xe0) {
			len = 3;
			sprintf(buf_cp50220, "%%0e%%%02x%%0f", c - 0x80);
		} else if (c >= 0x100) {
			const int j = ((((c & 0xff00) - (c >= 0xe000 ? 0xb000: 0x7000)) << 1) | ((c & 0xff) - (c & 0x80 ? 32: 31))) - ((c & 0xff) >= 159 ? 94: 0x100);
			len = 8;
			sprintf(buf_cp50220, "%%1b%%24%%42%%%02x%%%02x%%1b%%28%%42", j >> 8, j & 0xff);
		} else {
			len = 1;
			sprintf(buf_cp50220, "%%%02x", c);
		}
		printf("    \"%s (%d)\\r\\n\" { pass $test }\n", buf_cp50220, len);
	}

	printf("}\n");
}


static struct generator_entry entries[] = {
	{ "to_cp932", prologue_to_cp932, epilogue, to_cp932_visitor },
	{ "to_cp50220", prologue_to_cp50220, epilogue, to_cp50220_visitor },
	{ "to_cp50222", prologue_to_cp50222, epilogue, to_cp50222_visitor },
	{ "from_cp932", prologue_from_cp932, epilogue, from_cp932_visitor },
	{ NULL }
};

static const char cp932_txt[] = "CP932.TXT";

int main(int argc, char **argv)
{
	int retval = 0;
	FILE *fp;
	char buf[1024];
	struct generator_entry* gen;
	struct mappings map;

	if (argc <= 1) {
		fprintf(stderr, "usage: %s generator\n", argv[0]);
		return 255;
	}

	for (gen = entries;; ++gen) {
		if (!gen->name) {
			fprintf(stderr, "Unknown generator: %s\n", argv[1]);
			return 1;
		}
		if (strcmp(gen->name, argv[1]) == 0)
			break;
	}

    fp = fopen(cp932_txt, "r");
	if (!fp) {
		fprintf(stderr, "Failed to open %s\n", cp932_txt);
		return 2;
	}

	mappings_init(&map);

	while (fgets(buf, sizeof(buf), fp)) {
		const char *fields[16];
		char *p = buf;
		int field = 0;
		int cp_932, cp_uni;
		for (;;) {
			char *q = 0;
			int eol = 0;

			if (field >= sizeof(fields) / sizeof(*fields)) {
				fprintf(stderr, "Too many fields (incorrect file?)\n");
				retval = 3;
				goto out;
			}

			for (;;) {
				if (*p == '\0' || *p == '#' || *p == 0x0a) {
					eol = 1;
					break;
				} else if (*p != ' ' && *p != '\t') {
					break;
				}
				++p;
			}

			if (eol)
				break;

			q = p;

			for (;;) {
				if (*p == '\0' || *p == '#' || *p == 0x0a) {
					eol = 1;
					break;
				} else if (*p == ' ' || *p == '\t') {
					break;
				}
				++p;
			}

			*p = '\0';
			fields[field++] = q;

			if (eol)
				break;
			++p;
		}
		if (field == 0 || field == 1) {
			continue;
		} else if (field != 2) {
			fprintf(stderr, "Unexpected field count (expected 2, got %d)\n", field);
			retval = 3;
			goto out;
		}
		cp_932 = strtol(fields[0], NULL, 0);
		if (errno == ERANGE || errno == EINVAL) {
			fprintf(stderr, "Malformed field value: %s\n", fields[0]);
			retval = 4;
			goto out;
		}
		cp_uni = strtol(fields[1], NULL, 0);
		if (errno == ERANGE || errno == EINVAL) {
			fprintf(stderr, "Malformed field value: %s\n", fields[1]);
			retval = 4;
			goto out;
		}

		if (mappings_add(&map, cp_uni, cp_932)) {
			fprintf(stderr, "Too many mappings to the same Unicode codepoint (U+%06X)\n", cp_uni);
			retval = 4;
			goto out;
		}
	}

	{
		size_t i;
		printf("%s", gen->prologue);
		for (i = 0; i < map.n; ++i)
			gen->visitor(&map.entries[i]);
		printf("%s", gen->epilogue);
	}

out:
	mappings_destroy(&map);
	return retval;
}

/*
 * vim: sts=4 sw=4 ts=4 noet
 */
