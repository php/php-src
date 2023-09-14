/*
 * IR - Lightweight JIT Compilation Framework
 * (IR loader)
 * Copyright (C) 2005-2022 Zend by Perforce.
 * Authors: Dmitry Stogov <dmitry@php.net>
 *
 * This file is generated from "ir.g". Do not edit!
 *
 * To generate ir_load.c use llk <https://github.com/dstogov/llk>:
 * php llk.php ir.g
 */

#include "ir.h"
#include "ir_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifndef _WIN32
# include <unistd.h>
#endif

const unsigned char *yy_buf;
const unsigned char *yy_end;
const unsigned char *yy_pos;
const unsigned char *yy_text;
uint32_t yy_line;

typedef struct _ir_parser_ctx {
	ir_ctx    *ctx;
	uint32_t   undef_count;
	ir_strtab  var_tab;
} ir_parser_ctx;

static ir_strtab type_tab;
static ir_strtab op_tab;

#define IR_IS_UNRESOLVED(ref) \
	((ref) < (ir_ref)0xc0000000)
#define IR_ENCODE_UNRESOLVED_REF(ref, op) \
	((ir_ref)0xc0000000 - ((ref) * sizeof(ir_ref) + (op)))
#define IR_DECODE_UNRESOLVED_REF(ref) \
	((ir_ref)0xc0000000 - (ref))

static ir_ref ir_use_var(ir_parser_ctx *p, uint32_t n, const char *str, size_t len) {
	ir_ref ref;
	uint32_t len32;

	IR_ASSERT(len <= 0xffffffff);
	len32 = (uint32_t)len;
	ref = ir_strtab_find(&p->var_tab, str, len32);
	if (!ref) {
		p->undef_count++;
		/* create a linked list of unresolved references with header in "var_tab" */
		ref = IR_UNUSED; /* list terminator */
		ir_strtab_lookup(&p->var_tab, str, len32, IR_ENCODE_UNRESOLVED_REF(p->ctx->insns_count, n));
	} else if (IR_IS_UNRESOLVED(ref)) {
		/* keep the linked list of unresolved references with header in "var_tab" */
		/* "ref" keeps the tail of the list */
		ir_strtab_update(&p->var_tab, str, len32, IR_ENCODE_UNRESOLVED_REF(p->ctx->insns_count, n));
	}
	return ref;
}

static void ir_define_var(ir_parser_ctx *p, const char *str, size_t len, ir_ref ref) {
	ir_ref old_ref;
	uint32_t len32;

	IR_ASSERT(len <= 0xffffffff);
	len32 = (uint32_t)len;
	old_ref = ir_strtab_lookup(&p->var_tab, str, len32, ref);
	if (ref != old_ref) {
		if (IR_IS_UNRESOLVED(old_ref)) {
			p->undef_count--;
			/* update the linked list of unresolved references */
			do {
				ir_ref *ptr = ((ir_ref*)(p->ctx->ir_base)) + IR_DECODE_UNRESOLVED_REF(old_ref);
				old_ref = *ptr;
				*ptr = ref;
			} while (old_ref != IR_UNUSED);
			ir_strtab_update(&p->var_tab, str, len32, ref);
		} else {
			fprintf(stderr, "ERROR: Redefined variable `%.*s` on line %d\n", (int)len32, str, yy_line);
			exit(2);
		}
	}
}

static void report_undefined_var(const char *str, uint32_t len, ir_ref val)
{
	if (IR_IS_UNRESOLVED(val)) {
		fprintf(stderr, "ERROR: Undefined variable `%.*s`\n", (int)len, str);
	}
}

static void ir_check_indefined_vars(ir_parser_ctx *p)
{
	ir_strtab_apply(&p->var_tab, report_undefined_var);
	exit(2);
}

/* forward declarations */
static void yy_error(const char *msg);
static void yy_error_sym(const char *msg, int sym);

#define YYPOS cpos
#define YYEND cend

#define YY_EOF 0
#define YY__LBRACE 1
#define YY__SEMICOLON 2
#define YY__RBRACE 3
#define YY_FUNC 4
#define YY__LPAREN 5
#define YY__COMMA 6
#define YY__RPAREN 7
#define YY__COLON 8
#define YY__EQUAL 9
#define YY_FUNC_ADDR 10
#define YY__SLASH 11
#define YY_NULL 12
#define YY_INF 13
#define YY_NAN 14
#define YY__MINUS 15
#define YY_ID 16
#define YY_DECNUMBER 17
#define YY_HEXNUMBER 18
#define YY_FLOATNUMBER 19
#define YY_CHARACTER 20
#define YY_STRING 21
#define YY_EOL 22
#define YY_WS 23
#define YY_ONE_LINE_COMMENT 24
#define YY_COMMENT 25

static const char * sym_name[] = {
	"<EOF>",
	"{",
	";",
	"}",
	"func",
	"(",
	",",
	")",
	":",
	"=",
	"func_addr",
	"/",
	"null",
	"inf",
	"nan",
	"-",
	"<ID>",
	"<DECNUMBER>",
	"<HEXNUMBER>",
	"<FLOATNUMBER>",
	"<CHARACTER>",
	"<STRING>",
	"<EOL>",
	"<WS>",
	"<ONE_LINE_COMMENT>",
	"<COMMENT>",
	NULL
};

#define YY_IN_SET(sym, set, bitset) \
	(bitset[sym>>3] & (1 << (sym & 0x7)))

static int skip_EOL(int sym);
static int skip_WS(int sym);
static int skip_ONE_LINE_COMMENT(int sym);
static int skip_COMMENT(int sym);
static int get_sym(void);
static int parse_ir(int sym);
static int parse_ir_func(int sym, ir_parser_ctx *p);
static int parse_ir_func_prototype(int sym, ir_parser_ctx *p);
static int parse_ir_insn(int sym, ir_parser_ctx *p);
static int parse_type(int sym, uint8_t *t);
static int parse_func(int sym, uint8_t *op);
static int parse_val(int sym, ir_parser_ctx *p, uint8_t op, uint32_t n, ir_ref *ref);
static int parse_const(int sym, uint8_t t, ir_val *val);
static int parse_ID(int sym, const char **str, size_t *len);
static int parse_DECNUMBER(int sym, uint32_t t, ir_val *val);
static int parse_HEXNUMBER(int sym, uint32_t t, ir_val *val);
static int parse_FLOATNUMBER(int sym, uint32_t t, ir_val *val);
static int parse_CHARACTER(int sym, ir_val *val);
static int parse_STRING(int sym, const char **str, size_t *len);

static int get_skip_sym(void) {
	int ch;
	int ret;
	int accept = -1;
	const unsigned char *accept_pos;
	const unsigned char *cpos = yy_pos;
	const unsigned char *cend = yy_end;

_yy_state_start:
	yy_text = YYPOS;
	ch = *YYPOS;
	switch (ch) {
		case 'f':
			ch = *++YYPOS;
			if (ch != 'u') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != 'n') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != 'c') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != '_') {ret = YY_FUNC; goto _yy_tunnel_73;}
			ch = *++YYPOS;
			if (ch != 'a') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != 'd') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != 'd') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != 'r') goto _yy_tunnel_2;
			ret = YY_FUNC_ADDR;
			goto _yy_state_73;
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
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
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
		case '_':
			goto _yy_state_2;
		case 'i':
			ch = *++YYPOS;
			if (ch != 'n') goto _yy_tunnel_2;
			ch = *++YYPOS;
			if (ch != 'f') goto _yy_tunnel_2;
			ret = YY_INF;
			goto _yy_state_73;
		case 'n':
			ch = *++YYPOS;
			if (ch == 'a') {
				ch = *++YYPOS;
				if (ch != 'n') goto _yy_tunnel_2;
				ret = YY_NAN;
				goto _yy_state_73;
			} else if (ch == 'u') {
				ch = *++YYPOS;
				if (ch != 'l') goto _yy_tunnel_2;
				ch = *++YYPOS;
				if (ch != 'l') goto _yy_tunnel_2;
				ret = YY_NULL;
				goto _yy_state_73;
			} else {
				goto _yy_tunnel_2;
			}
		case '(':
			YYPOS++;
			ret = YY__LPAREN;
			goto _yy_fin;
		case ',':
			YYPOS++;
			ret = YY__COMMA;
			goto _yy_fin;
		case ')':
			YYPOS++;
			ret = YY__RPAREN;
			goto _yy_fin;
		case ':':
			YYPOS++;
			ret = YY__COLON;
			goto _yy_fin;
		case '{':
			YYPOS++;
			ret = YY__LBRACE;
			goto _yy_fin;
		case '=':
			YYPOS++;
			ret = YY__EQUAL;
			goto _yy_fin;
		case '-':
			ch = *++YYPOS;
			accept = YY__MINUS;
			accept_pos = yy_pos;
			if ((ch >= '0' && ch <= '9')) {
				goto _yy_state_13;
			} else if (ch == '.') {
				goto _yy_state_14;
			} else {
				ret = YY__MINUS;
				goto _yy_fin;
			}
		case '0':
			ch = *++YYPOS;
			if (ch != 'x') goto _yy_tunnel_13;
			ch = *++YYPOS;
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
				goto _yy_state_50;
			} else {
				goto _yy_state_error;
			}
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			goto _yy_state_13;
		case '.':
			goto _yy_state_14;
		case '\'':
			goto _yy_state_15;
		case '"':
			goto _yy_state_16;
		case '/':
			ch = *++YYPOS;
			accept = YY__SLASH;
			accept_pos = yy_pos;
			if (ch == '*') {
				goto _yy_state_41;
			} else if (ch == '/') {
				goto _yy_state_23;
			} else {
				ret = YY__SLASH;
				goto _yy_fin;
			}
		case ';':
			YYPOS++;
			ret = YY__SEMICOLON;
			goto _yy_fin;
		case '}':
			YYPOS++;
			ret = YY__RBRACE;
			goto _yy_fin;
		case '\r':
			ch = *++YYPOS;
			if (ch == '\n') {
				yy_line++;
				YYPOS++;
				ret = YY_EOL;
				goto _yy_fin;
			} else {
				ret = YY_EOL;
				goto _yy_fin;
			}
		case '\n':
			yy_line++;
			YYPOS++;
			ret = YY_EOL;
			goto _yy_fin;
		case ' ':
		case '\t':
		case '\f':
		case '\v':
			goto _yy_state_22;
		case '#':
			goto _yy_state_23;
		case '\0':
			if (ch == 0 && YYPOS < YYEND) goto _yy_state_error;
			YYPOS++;
			ret = YY_EOF;
			goto _yy_fin;
		default:
			goto _yy_state_error;
	}
_yy_state_2:
	ch = *++YYPOS;
_yy_tunnel_2:
	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {
		goto _yy_state_2;
	} else {
		ret = YY_ID;
		goto _yy_fin;
	}
_yy_state_13:
	ch = *++YYPOS;
_yy_tunnel_13:
	accept = YY_DECNUMBER;
	accept_pos = yy_pos;
	if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_13;
	} else if (ch == '.') {
		goto _yy_state_31;
	} else if (ch == 'E' || ch == 'e') {
		goto _yy_state_32;
	} else {
		ret = YY_DECNUMBER;
		goto _yy_fin;
	}
_yy_state_14:
	ch = *++YYPOS;
	if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_31;
	} else {
		goto _yy_state_error;
	}
_yy_state_15:
	ch = *++YYPOS;
	if (ch == '\\') {
		ch = *++YYPOS;
		if (YYPOS < YYEND) {
			if (ch == '\n') {
				yy_line++;
			}
			goto _yy_state_15;
		} else {
			goto _yy_state_error;
		}
	} else if (ch == '\'') {
		YYPOS++;
		ret = YY_CHARACTER;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= '&' || (ch >= '(' && ch <= '[') || ch >= ']')) {
		if (ch == '\n') {
			yy_line++;
		}
		goto _yy_state_15;
	} else {
		goto _yy_state_error;
	}
_yy_state_16:
	ch = *++YYPOS;
	if (ch == '\\') {
		ch = *++YYPOS;
		if (YYPOS < YYEND) {
			if (ch == '\n') {
				yy_line++;
			}
			goto _yy_state_16;
		} else {
			goto _yy_state_error;
		}
	} else if (ch == '"') {
		YYPOS++;
		ret = YY_STRING;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= '!' || (ch >= '#' && ch <= '[') || ch >= ']')) {
		if (ch == '\n') {
			yy_line++;
		}
		goto _yy_state_16;
	} else {
		goto _yy_state_error;
	}
_yy_state_22:
	ch = *++YYPOS;
	if (ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ') {
		goto _yy_state_22;
	} else {
		ret = YY_WS;
		goto _yy_fin;
	}
_yy_state_23:
	ch = *++YYPOS;
	if (ch == '\r') {
		ch = *++YYPOS;
		if (ch == '\n') {
			yy_line++;
			YYPOS++;
			ret = YY_ONE_LINE_COMMENT;
			goto _yy_fin;
		} else {
			ret = YY_ONE_LINE_COMMENT;
			goto _yy_fin;
		}
	} else if (ch == '\n') {
		yy_line++;
		YYPOS++;
		ret = YY_ONE_LINE_COMMENT;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= '\t' || ch == '\v' || ch == '\f' || ch >= '\016')) {
		goto _yy_state_23;
	} else {
		goto _yy_state_error;
	}
_yy_state_31:
	ch = *++YYPOS;
	accept = YY_FLOATNUMBER;
	accept_pos = yy_pos;
	if (ch == 'E' || ch == 'e') {
		goto _yy_state_32;
	} else if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_31;
	} else {
		ret = YY_FLOATNUMBER;
		goto _yy_fin;
	}
_yy_state_32:
	ch = *++YYPOS;
	if (ch == '+' || ch == '-') {
		ch = *++YYPOS;
		if ((ch >= '0' && ch <= '9')) {
			goto _yy_state_53;
		} else {
			goto _yy_state_error;
		}
	} else if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_53;
	} else {
		goto _yy_state_error;
	}
_yy_state_41:
	ch = *++YYPOS;
_yy_tunnel_41:
	if (ch == '*') {
		ch = *++YYPOS;
		if (ch != '/') goto _yy_tunnel_41;
		YYPOS++;
		ret = YY_COMMENT;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= ')' || ch >= '+')) {
		if (ch == '\n') {
			yy_line++;
		}
		goto _yy_state_41;
	} else {
		goto _yy_state_error;
	}
_yy_state_50:
	ch = *++YYPOS;
	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
		goto _yy_state_50;
	} else {
		ret = YY_HEXNUMBER;
		goto _yy_fin;
	}
_yy_state_53:
	ch = *++YYPOS;
	if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_53;
	} else {
		ret = YY_FLOATNUMBER;
		goto _yy_fin;
	}
_yy_state_73:
	ch = *++YYPOS;
_yy_tunnel_73:
	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {
		goto _yy_state_2;
	} else {
		goto _yy_fin;
	}
_yy_state_error:
	if (accept >= 0) {
		yy_pos = accept_pos;
		return accept;
	}
	if (YYPOS >= YYEND) {
		yy_error("unexpected <EOF>");
	} else if (YYPOS == yy_text) {
		yy_error("unexpected character 'escape_char(ch)'");
	} else {
		yy_error("unexpected sequence 'escape_string(yy_text, 1 + YYPOS - yy_text))'");
	}
	YYPOS++;
	goto _yy_state_start;
_yy_fin:
	yy_pos = YYPOS;
	return ret;
}

static int skip_EOL(int sym) {
	if (sym != YY_EOL) {
		yy_error_sym("<EOL> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int skip_WS(int sym) {
	if (sym != YY_WS) {
		yy_error_sym("<WS> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int skip_ONE_LINE_COMMENT(int sym) {
	if (sym != YY_ONE_LINE_COMMENT) {
		yy_error_sym("<ONE_LINE_COMMENT> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int skip_COMMENT(int sym) {
	if (sym != YY_COMMENT) {
		yy_error_sym("<COMMENT> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int get_sym(void) {
	int sym;
	sym = get_skip_sym();
	while (sym == YY_EOL || sym == YY_WS || sym == YY_ONE_LINE_COMMENT || sym == YY_COMMENT) {
		if (sym == YY_EOL) {
			sym = skip_EOL(sym);
		} else if (sym == YY_WS) {
			sym = skip_WS(sym);
		} else if (sym == YY_ONE_LINE_COMMENT) {
			sym = skip_ONE_LINE_COMMENT(sym);
		} else {
			sym = skip_COMMENT(sym);
		}
	}
	return sym;
}

static int parse_ir(int sym) {
	ir_parser_ctx p;
	do {
		sym = parse_ir_func_prototype(sym, &p);
		sym = parse_ir_func(sym, &p);
	} while (sym == YY_FUNC);
	return sym;
}

static int parse_ir_func(int sym, ir_parser_ctx *p) {
	if (sym != YY__LBRACE) {
		yy_error_sym("'{' expected, got", sym);
	}
	sym = get_sym();
	while (sym == YY_ID) {
		sym = parse_ir_insn(sym, p);
		if (sym != YY__SEMICOLON) {
			yy_error_sym("';' expected, got", sym);
		}
		sym = get_sym();
	}
	if (sym != YY__RBRACE) {
		yy_error_sym("'}' expected, got", sym);
	}
	sym = get_sym();
	return sym;
}

static int parse_ir_func_prototype(int sym, ir_parser_ctx *p) {
	const char *name;
	size_t len;
	uint8_t t = 0;
	if (sym != YY_FUNC) {
		yy_error_sym("'func' expected, got", sym);
	}
	sym = get_sym();
	sym = parse_ID(sym, &name, &len);
	if (sym != YY__LPAREN) {
		yy_error_sym("'(' expected, got", sym);
	}
	sym = get_sym();
	if (sym == YY_ID) {
		sym = parse_type(sym, &t);
		while (sym == YY__COMMA) {
			sym = get_sym();
			sym = parse_type(sym, &t);
		}
	}
	if (sym != YY__RPAREN) {
		yy_error_sym("')' expected, got", sym);
	}
	sym = get_sym();
	if (sym != YY__COLON) {
		yy_error_sym("':' expected, got", sym);
	}
	sym = get_sym();
	sym = parse_type(sym, &t);
	return sym;
}

static int parse_ir_insn(int sym, ir_parser_ctx *p) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt0;
	const char *str, *str2 = NULL, *func;
	size_t len, len2 = 0, func_len;
	uint8_t op;
	uint8_t t = 0;
	ir_ref op1 = IR_UNUSED;
	ir_ref op2 = IR_UNUSED;
	ir_ref op3 = IR_UNUSED;
	ir_ref ref;
	ir_val val;
	ir_val count;
	ir_val flags;
	int32_t n;
	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	alt0 = -2;
	sym2 = sym;
	if (sym2 == YY_ID) {
		sym2 = get_sym();
		goto _yy_state_0_1;
	} else {
		yy_error_sym("unexpected", sym2);
	}
_yy_state_0_1:
	if (sym2 == YY_ID) {
		alt0 = 1;
		goto _yy_state_0;
	} else if (sym2 == YY__EQUAL) {
		alt0 = 5;
		goto _yy_state_0;
	} else {
		yy_error_sym("unexpected", sym2);
	}
_yy_state_0:
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	if (alt0 == 1) {
		sym = parse_type(sym, &t);
		sym = parse_ID(sym, &str, &len);
		if (sym == YY__COMMA) {
			sym = get_sym();
			sym = parse_ID(sym, &str2, &len2);
		}
	} else if (alt0 == 5) {
		sym = parse_ID(sym, &str, &len);
	} else {
		yy_error_sym("unexpected", sym);
	}
	if (sym != YY__EQUAL) {
		yy_error_sym("'=' expected, got", sym);
	}
	sym = get_sym();
	switch (sym) {
		case YY_DECNUMBER:
		case YY_HEXNUMBER:
		case YY_FLOATNUMBER:
		case YY_CHARACTER:
		case YY_INF:
		case YY_NAN:
		case YY__MINUS:
			val.u64 = 0;
			sym = parse_const(sym, t, &val);
			ref = ir_const(p->ctx, val, t);
			break;
		case YY_FUNC:
			sym = get_sym();
			if (sym != YY__LPAREN) {
				yy_error_sym("'(' expected, got", sym);
			}
			sym = get_sym();
			sym = parse_ID(sym, &func, &func_len);
			flags.u64 = 0;
			if (sym == YY__COMMA) {
				sym = get_sym();
				sym = parse_DECNUMBER(sym, IR_U16, &flags);
			}
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
			ref = ir_const_func(p->ctx, ir_strl(p->ctx, func, func_len), flags.u16);
			break;
		case YY_FUNC_ADDR:
			sym = get_sym();
			if (sym != YY__LPAREN) {
				yy_error_sym("'(' expected, got", sym);
			}
			sym = get_sym();
			if (sym == YY_DECNUMBER) {
				sym = parse_DECNUMBER(sym, IR_ADDR, &val);
			} else if (sym == YY_HEXNUMBER) {
				sym = parse_HEXNUMBER(sym, IR_ADDR, &val);
			} else {
				yy_error_sym("unexpected", sym);
			}
			flags.u64 = 0;
			if (sym == YY__COMMA) {
				sym = get_sym();
				sym = parse_DECNUMBER(sym, IR_U16, &flags);
			}
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
			ref = ir_const_func_addr(p->ctx, val.addr, flags.u16);
			break;
		case YY_STRING:
			sym = parse_STRING(sym, &func, &func_len);
			ref = ir_const_str(p->ctx, ir_strl(p->ctx, func, func_len));
			break;
		case YY_ID:
			sym = parse_func(sym, &op);
			if (sym == YY__SLASH) {
				sym = get_sym();
				sym = parse_DECNUMBER(sym, IR_I32, &count);
				if (op == IR_PHI || op == IR_SNAPSHOT) count.i32++;
				if (op == IR_CALL || op == IR_TAILCALL) count.i32+=2;
				if (count.i32 < 0 || count.i32 > 255) yy_error("bad bumber of operands");
				ref = ir_emit_N(p->ctx, IR_OPT(op, t), count.i32);
				if (sym == YY__LPAREN) {
					sym = get_sym();
					if (sym == YY_ID || sym == YY_STRING || sym == YY_DECNUMBER || sym == YY_NULL) {
						sym = parse_val(sym, p, op, 1, &op1);
						n = 1;
						if (n > count.i32) yy_error("too many operands");
						ir_set_op(p->ctx, ref, n, op1);
						while (sym == YY__COMMA) {
							sym = get_sym();
							sym = parse_val(sym, p, op, n, &op1);
							n++;
							if (n > count.i32) yy_error("too many operands");
							ir_set_op(p->ctx, ref, n, op1);
						}
					}
					if (sym != YY__RPAREN) {
						yy_error_sym("')' expected, got", sym);
					}
					sym = get_sym();
				}
			} else if (sym == YY__LPAREN || sym == YY__SEMICOLON) {
				n = 0;
				if (sym == YY__LPAREN) {
					sym = get_sym();
					if (sym == YY_ID || sym == YY_STRING || sym == YY_DECNUMBER || sym == YY_NULL) {
						sym = parse_val(sym, p, op, 1, &op1);
						n = 1;
						if (sym == YY__COMMA) {
							sym = get_sym();
							sym = parse_val(sym, p, op, 2, &op2);
							n = 2;
							if (sym == YY__COMMA) {
								sym = get_sym();
								sym = parse_val(sym, p, op, 3, &op3);
								n = 3;
							}
						}
					}
					if (sym != YY__RPAREN) {
						yy_error_sym("')' expected, got", sym);
					}
					sym = get_sym();
				}
				if (IR_IS_FOLDABLE_OP(op)
				 && !IR_IS_UNRESOLVED(op1)
				 && !IR_IS_UNRESOLVED(op2)
				 && !IR_IS_UNRESOLVED(op3)) {
					ref = ir_fold(p->ctx, IR_OPT(op, t), op1, op2, op3);
				} else {
					uint32_t opt;

					if (!IR_OP_HAS_VAR_INPUTS(ir_op_flags[op])) {
						opt = IR_OPT(op, t);
					} else {
						opt = IR_OPTX(op, t, n);
					}
					ref = ir_emit(p->ctx, opt, op1, op2, op3);
				}
			} else {
				yy_error_sym("unexpected", sym);
			}
			break;
		default:
			yy_error_sym("unexpected", sym);
	}
	ir_define_var(p, str, len, ref);
	if (str2) ir_define_var(p, str2, len2, ref);
	return sym;
}

static int parse_type(int sym, uint8_t *t) {
	const char *str;
	size_t len;
	ir_ref ref;
	sym = parse_ID(sym, &str, &len);
	IR_ASSERT(len <= 0xffffffff);
	ref = ir_strtab_find(&type_tab, str, (uint32_t)len);
	if (!ref) yy_error("invalid type");
	*t = ref;
	return sym;
}

static int parse_func(int sym, uint8_t *op) {
	const char *str;
	size_t len;
	ir_ref ref;
	sym = parse_ID(sym, &str, &len);
	IR_ASSERT(len <= 0xffffffff);
	ref = ir_strtab_find(&op_tab, str, (uint32_t)len);
	if (!ref) yy_error("invalid op");
	*op = ref - 1;
	return sym;
}

static int parse_val(int sym, ir_parser_ctx *p, uint8_t op, uint32_t n, ir_ref *ref) {
	const char *str;
	size_t len;
	ir_val val;
	uint32_t kind = IR_OPND_KIND(ir_op_flags[op], n);
	if (sym == YY_ID) {
		sym = parse_ID(sym, &str, &len);
		if (!IR_IS_REF_OPND_KIND(kind)) yy_error("unexpected reference");
		*ref = ir_use_var(p, n, str, len);
	} else if (sym == YY_STRING) {
		sym = parse_STRING(sym, &str, &len);
		if (kind != IR_OPND_STR) yy_error("unexpected string");
		*ref = ir_strl(p->ctx, str, len);
	} else if (sym == YY_DECNUMBER) {
		sym = parse_DECNUMBER(sym, IR_I32, &val);
		if (kind != IR_OPND_NUM && kind != IR_OPND_PROB) yy_error("unexpected number");
		if (val.i64 < 0 || val.i64 > 0x7fffffff) yy_error("number out of range");
		*ref = val.i32;
	} else if (sym == YY_NULL) {
		sym = get_sym();
		*ref = IR_UNUSED;
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_const(int sym, uint8_t t, ir_val *val) {
	switch (sym) {
		case YY_DECNUMBER:
			sym = parse_DECNUMBER(sym, t, val);
			break;
		case YY_HEXNUMBER:
			sym = parse_HEXNUMBER(sym, t, val);
			break;
		case YY_FLOATNUMBER:
			sym = parse_FLOATNUMBER(sym, t, val);
			break;
		case YY_CHARACTER:
			sym = parse_CHARACTER(sym, val);
			break;
		case YY_INF:
			sym = get_sym();
			if (t == IR_DOUBLE) val->d = INFINITY; else val->f = INFINITY;
			break;
		case YY_NAN:
			sym = get_sym();
			if (t == IR_DOUBLE) val->d = NAN; else val->f = NAN;
			break;
		case YY__MINUS:
			sym = get_sym();
			if (sym != YY_INF) {
				yy_error_sym("'inf' expected, got", sym);
			}
			sym = get_sym();
			if (t == IR_DOUBLE) val->d = -INFINITY; else val->f = -INFINITY;
			break;
		default:
			yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_ID(int sym, const char **str, size_t *len) {
	if (sym != YY_ID) {
		yy_error_sym("<ID> expected, got", sym);
	}
	*str = (const char*)yy_text; *len = yy_pos - yy_text;
	sym = get_sym();
	return sym;
}

static int parse_DECNUMBER(int sym, uint32_t t, ir_val *val) {
	if (sym != YY_DECNUMBER) {
		yy_error_sym("<DECNUMBER> expected, got", sym);
	}
	if (t == IR_DOUBLE) val->d = atof((const char*)yy_text);
	else if (t == IR_FLOAT) val->f = strtof((const char*)yy_text, NULL);
	else if (IR_IS_TYPE_SIGNED(t)) val->i64 = atoll((const char*)yy_text);
	else val->u64 = strtoull((const char*)yy_text, NULL, 10);
	sym = get_sym();
	return sym;
}

static int parse_HEXNUMBER(int sym, uint32_t t, ir_val *val) {
	if (sym != YY_HEXNUMBER) {
		yy_error_sym("<HEXNUMBER> expected, got", sym);
	}
	val->u64 = strtoull((const char*)yy_text + 2, NULL, 16);
	sym = get_sym();
	return sym;
}

static int parse_FLOATNUMBER(int sym, uint32_t t, ir_val *val) {
	if (sym != YY_FLOATNUMBER) {
		yy_error_sym("<FLOATNUMBER> expected, got", sym);
	}
	if (t == IR_DOUBLE) val->d = atof((const char*)yy_text); else val->f = strtof((const char*)yy_text, NULL);
	sym = get_sym();
	return sym;
}

static int parse_CHARACTER(int sym, ir_val *val) {
	if (sym != YY_CHARACTER) {
		yy_error_sym("<CHARACTER> expected, got", sym);
	}
	if ((char)yy_text[1] != '\\') {
			val->i64 = (char)yy_text[1];
		} else if ((char)yy_text[2] == '\\') {
			val->i64 = '\\';
		} else if ((char)yy_text[2] == 'r') {
			val->i64 = '\r';
		} else if ((char)yy_text[2] == 'n') {
			val->i64 = '\n';
		} else if ((char)yy_text[2] == 't') {
			val->i64 = '\t';
		} else if ((char)yy_text[2] == '0') {
			val->i64 = '\0';
		} else {
			IR_ASSERT(0);
		}
	sym = get_sym();
	return sym;
}

static int parse_STRING(int sym, const char **str, size_t *len) {
	if (sym != YY_STRING) {
		yy_error_sym("<STRING> expected, got", sym);
	}
	*str = (const char*)yy_text + 1; *len = yy_pos - yy_text - 2;
	sym = get_sym();
	return sym;
}

static void parse(void) {
	int sym;

	yy_pos = yy_text = yy_buf;
	yy_line = 1;
	sym = parse_ir(get_sym());
	if (sym != YY_EOF) {
		yy_error_sym("<EOF> expected, got", sym);
	}
}

static void yy_error(const char *msg) {
	fprintf(stderr, "ERROR: %s at line %d\n", msg, yy_line);
	exit(2);
}

static void yy_error_sym(const char *msg, int sym) {
	fprintf(stderr, "ERROR: %s '%s' at line %d\n", msg, sym_name[sym], yy_line);
	exit(2);
}

int ir_load(ir_ctx *ctx, FILE *f) {
	ir_parser_ctx p;
	int sym;
	long pos, end;

	p.ctx = ctx;
	p.undef_count = 0;
	ir_strtab_init(&p.var_tab, 256, 4096);

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);
	yy_buf = alloca(end - pos + 1);
	yy_end = yy_buf + (end - pos);
	fread((void*)yy_buf, (end - pos), 1, f);
	*(unsigned char*)yy_end = 0;

	yy_pos = yy_text = yy_buf;
	yy_line = 1;
	sym = parse_ir_func(get_sym(), &p);
	if (sym != YY_EOF) {
		yy_error_sym("<EOF> expected, got", sym);
	}
	if (p.undef_count) {
		ir_check_indefined_vars(&p);
	}

	ir_strtab_free(&p.var_tab);

	return 1;
}

void ir_loader_init(void)
{
	ir_ref i;

	ir_strtab_init(&type_tab, IR_LAST_OP, 0);
	for (i = 1; i < IR_LAST_TYPE; i++) {
		ir_strtab_lookup(&type_tab, ir_type_cname[i], (uint32_t)strlen(ir_type_cname[i]), i);
	}

	ir_strtab_init(&op_tab, IR_LAST_OP, 0);
	for (i = 0; i < IR_LAST_OP; i++) {
		ir_strtab_lookup(&op_tab, ir_op_name[i], (uint32_t)strlen(ir_op_name[i]), i + 1);
	}
}

void ir_loader_free(void)
{
	ir_strtab_free(&type_tab);
	ir_strtab_free(&op_tab);
}
