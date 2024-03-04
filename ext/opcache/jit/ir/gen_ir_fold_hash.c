/*
 * IR - Lightweight JIT Compilation Framework
 * (Folding engine generator)
 * Copyright (C) 2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * Based on Mike Pall's implementation for LuaJIT.
 */

#include "ir.h"
#include <string.h>

#include "ir_strtab.c"

#define MAX_RULES 2048
#define MAX_SLOTS (MAX_RULES * 4)

static ir_strtab strtab;

void print_hash(uint32_t *mask, uint32_t count)
{
	uint32_t i;

	printf("static const uint32_t _ir_fold_hash[%d] = {\n", count);
	for (i = 0; i < count; i++) {
		printf("\t0x%08x,\n", mask[i]);
	}
	printf("};\n\n");
}

#if 0
static uint32_t hash_shl2(uint32_t mask, uint32_t r1, uint32_t r2)
{
	return ((mask << r1) - mask) << r2;
}
#else
#define ir_rol(x, n)	(((x)<<(n)) | ((x)>>(-(int)(n)&(8*sizeof(x)-1))))
#define ir_ror(x, n)	(((x)<<(-(int)(n)&(8*sizeof(x)-1))) | ((x)>>(n)))

static uint32_t hash_rol2(uint32_t mask, uint32_t r1, uint32_t r2)
{
	return ir_rol((ir_rol(mask, r1) - mask), r2);
}
#endif

/* Find a perfect hash function */
int find_hash(uint32_t *mask, uint32_t count)
{
	uint32_t hash[MAX_SLOTS];
	uint32_t n, r1, r2, i, h;

	for (n = (count | 1); n < MAX_SLOTS; n += 2) {
		for (r1 = 0; r1 < 31; r1++) {
			for (r2 = 0; r2 < 32; r2++) {
#if 0
				memset(hash, 0, n * sizeof(uint32_t));
				for (i = 0; i < count; i++) {
					h = hash_shl2(mask[i] & 0x1fffff, r1, r2) % n;
					if (hash[h]) break; /* collision */
					hash[h] = mask[i];
				}
				if (i == count) {
					print_hash(hash, n);
					printf("static uint32_t _ir_fold_hashkey(uint32_t h)\n{\n\treturn (((h << %d) - h) << %d) %% %d;\n}\n", r1, r2, n);
					return 1;
				}
#else
				memset(hash, 0, n * sizeof(uint32_t));
				for (i = 0; i < count; i++) {
					h = hash_rol2(mask[i] & 0x1fffff, r1, r2) % n;
					if (hash[h]) break; /* collision */
					hash[h] = mask[i];
				}
				if (i == count) {
					print_hash(hash, n);
					printf("static uint32_t _ir_fold_hashkey(uint32_t h)\n{\nreturn ir_rol32((ir_rol32(h, %d) - h), %d) %% %d;\n}\n", r1, r2, n);
					return 1;
				}
#endif
			}
		}
	}

	hash[0] = 0;
	print_hash(hash, 1);
	printf("static uint32_t _ir_fold_hashkey(uint32_t h)\n{\n\treturn 0;\n}\n");
	return 0;
}

static int find_op(const char *s, size_t len)
{
	return ir_strtab_find(&strtab, s, (uint8_t)len) - 1;
}

static int parse_rule(const char *buf)
{
	const char *p = buf + sizeof("IR_FOLD(") - 1;
	const char *q;
	int op, mask;

	while (*p == ' ' || *p == '\t') {
		p++;
	}
	if (*p < 'A' || *p > 'Z') {
		return 0;
	}
	q = p + 1;
	while ((*q >= 'A' && *q <= 'Z')
	 || (*q >= '0' && *q <= '9')
	 || *q == '_') {
		q++;
	}
	op = find_op(p, q - p);
	if (op < 0) {
		return 0;
	}
	mask = op;

	while (*q == ' ' || *q == '\t') {
		q++;
	}
	if (*q == ')') {
		return mask; /* unused operands */
	} else if (*q != '(') {
		return 0;
	}

	p = q + 1;
	while (*p == ' ' || *p == '\t') {
		p++;
	}
	if (*p == '_') {
		q = p + 1;
	} else if (*p >= 'A' && *p <= 'Z') {
		q = p + 1;
		while ((*q >= 'A' && *q <= 'Z')
		 || (*q >= '0' && *q <= '9')
		 || *q == '_') {
			q++;
		}
		op = find_op(p, q - p);
		if (op < 0) {
			return 0;
		}
		mask |= op << 7;
	} else {
		return 0;
	}

	while (*q == ' ' || *q == '\t') {
		q++;
	}
	if (*q == ')') {
		return mask; /* unused op2 */
	} else if (*q != ',') {
		return 0;
	}

	p = q + 1;
	while (*p == ' ' || *p == '\t') {
		p++;
	}
	if (*p == '_') {
		q = p + 1;
	} else if (*p >= 'A' && *p <= 'Z') {
		q = p + 1;
		while ((*q >= 'A' && *q <= 'Z')
		 || (*q >= '0' && *q <= '9')
		 || *q == '_') {
			q++;
		}
		op = find_op(p, q - p);
		if (op < 0) {
			return 0;
		}
		mask |= op << 14;
	} else {
		return 0;
	}

	while (*q == ' ' || *q == '\t') {
		q++;
	}
	if (*q != ')') {
		return 0;
	}

	q++;
	while (*q == ' ' || *q == '\t') {
		q++;
	}
	if (*q != ')') {
		return 0;
	}

	return mask;
}

int main()
{
	char buf[4096];
	FILE *f = stdin;
	int line = 0;
	int rules = 0;
	int i;
	uint32_t mask[MAX_RULES];
	uint32_t rule[MAX_RULES];

	ir_strtab_init(&strtab, IR_LAST_OP, 0);

#define IR_OP_ADD(name, flags, op1, op2, op3) \
	ir_strtab_lookup(&strtab, #name, sizeof(#name) - 1, IR_ ## name + 1);

	IR_OPS(IR_OP_ADD)

	while (fgets(buf, sizeof(buf) - 1, f)) {
		size_t len = strlen(buf);
		if (len > 0 && (buf[len - 1] == '\r' || buf[len - 1] == '\n')) {
			buf[len - 1] = 0;
			len--;
			line++;
		}
		if (len >= sizeof("IR_FOLD(")-1
		 && memcmp(buf, "IR_FOLD(", sizeof("IR_FOLD(")-1) == 0) {
		    if (rules >= MAX_RULES) {
				fprintf(stderr, "ERROR: Too many rules\n");
				return 1;
			}
			i = parse_rule(buf);
			if (!i) {
				fprintf(stderr, "ERROR: Incorrect '%s' rule on line %d\n", buf, line);
				return 1;
			}
			// TODO: few masks may share the same rule ???
			rule[rules] = line;
			mask[rules] = i | (rules << 21);
			rules++;
		}
	}
	ir_strtab_free(&strtab);

#if 0
	for (i = 0; i < rules; i++) {
		printf("0x%08x\n", mask[i]);
	}
#endif

	printf("/* This file is generated from \"ir_fold.h\". Do not edit! */\n\n");
	printf("typedef enum _ir_fold_rule_id {\n");
	for (i = 0; i < rules; i++) {
		printf("\tIR_RULE_%d,\n", rule[i]);
	}
	printf("\t_IR_RULE_LAST\n");
	printf("} ir_fold_rule_id;\n\n");

	if (!find_hash(mask, rules)) {
		fprintf(stderr, "ERROR: Cannot find a good hash function\n");
		return 1;
	}

	return 0;
}
