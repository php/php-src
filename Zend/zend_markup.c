/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright © Zend Technologies Ltd., a subsidiary company of          |
   |     Perforce Software, Inc., and Contributors.                       |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Liam Hammett                                                 |
   +----------------------------------------------------------------------+
*/

/* Native markup (RFC: Native Markup Expressions) compile-time support:
 *
 *   - text normalization and character-reference decoding, called by the
 *     lexer (Zend/zend_language_scanner.l) on markup text and literal
 *     attribute values;
 *   - AST lowering, called by the grammar actions
 *     (Zend/zend_language_parser.y) to turn markup elements into ordinary
 *     `new \Html\Element(...)` / `\Html\render_component(...)` ASTs, so no
 *     new AST node kinds or zend_compile.c support are needed.
 *
 * The lexing mechanics themselves (scanner states, operand-position
 * detection) stay in the scanner; everything here is pure data-to-data. */

#include "zend.h"
#include "zend_ast.h"
#include "zend_compile.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"
#include "zend_markup.h"
#include "zend_markup_entities.h"

/* Native markup whitespace normalization (RFC §7), following the JSX/Babel rules:
 *   - split the text node into lines;
 *   - trim leading whitespace on every line but the first, and trailing
 *     whitespace on every line but the last;
 *   - drop lines that are empty after trimming;
 *   - join the surviving lines with a single space.
 * The net effect: indentation/newlines between block elements vanish, while a
 * meaningful single space between inline content is preserved. Returns a new
 * zend_string (possibly empty — the caller drops empty text nodes). */
typedef struct { const char *p; size_t n; } zend_markup_line;

static zend_always_inline zend_markup_line *zend_markup_lines_grow(
	zend_markup_line *lines, zend_markup_line *stackbuf, uint32_t count, uint32_t *cap)
{
	*cap *= 2;
	zend_markup_line *grown = emalloc(*cap * sizeof(zend_markup_line));
	memcpy(grown, lines, count * sizeof(zend_markup_line));
	if (lines != stackbuf) {
		efree(lines);
	}
	return grown;
}

zend_string *zend_markup_normalize_text(const char *text, size_t len)
{
	zend_markup_line stackbuf[32];
	zend_markup_line *lines = stackbuf;
	uint32_t cap = 32, count = 0;
	size_t pos = 0;

	/* Split on \r\n | \n | \r (matching JS String.split), keeping a trailing
	 * empty line after a final break so the previous line counts as "not last". */
	for (;;) {
		size_t e = pos;
		while (e < len && text[e] != '\n' && text[e] != '\r') {
			e++;
		}
		if (count == cap) {
			lines = zend_markup_lines_grow(lines, stackbuf, count, &cap);
		}
		lines[count].p = text + pos;
		lines[count].n = e - pos;
		count++;

		if (e >= len) {
			break;
		}
		if (text[e] == '\r' && e + 1 < len && text[e + 1] == '\n') {
			e++;
		}
		pos = e + 1;
		if (pos == len) {
			if (count == cap) {
				lines = zend_markup_lines_grow(lines, stackbuf, count, &cap);
			}
			lines[count].p = text + len;
			lines[count].n = 0;
			count++;
			break;
		}
	}

	/* Index of the last line containing a non-space/tab char (default 0, as in Babel). */
	uint32_t last_nonblank = 0;
	for (uint32_t k = 0; k < count; k++) {
		for (size_t x = 0; x < lines[k].n; x++) {
			if (lines[k].p[x] != ' ' && lines[k].p[x] != '\t') {
				last_nonblank = k;
				break;
			}
		}
	}

	/* Output is never longer than the input. */
	zend_string *result = zend_string_alloc(len, 0);
	char *out = ZSTR_VAL(result);
	size_t outlen = 0;

	for (uint32_t k = 0; k < count; k++) {
		const char *p = lines[k].p;
		size_t s = 0, en = lines[k].n;
		bool is_first = (k == 0);
		bool is_last = (k == count - 1);

		if (!is_first) {
			while (s < en && (p[s] == ' ' || p[s] == '\t')) s++;
		}
		if (!is_last) {
			while (en > s && (p[en - 1] == ' ' || p[en - 1] == '\t')) en--;
		}
		if (en > s) {
			for (size_t x = s; x < en; x++) {
				out[outlen++] = (p[x] == '\t') ? ' ' : p[x];
			}
			if (k != last_nonblank) {
				out[outlen++] = ' ';
			}
		}
	}

	if (lines != stackbuf) {
		efree(lines);
	}

	if (outlen == 0) {
		zend_string_release(result);
		return ZSTR_EMPTY_ALLOC();
	}
	ZSTR_LEN(result) = outlen;
	out[outlen] = '\0';
	return result;
}

/* Native markup character references (RFC §7). Markup text and literal
 * attribute values decode HTML character references at compile time: the
 * frozen WHATWG named set (semicolon-terminated forms only; the standard
 * guarantees the list will never grow) plus numeric &#123; / &#x1F600; forms.
 * Decoding is lenient like HTML and JSX: anything that does not parse as a
 * reference — a bare "&", an unknown name, an out-of-range or surrogate
 * codepoint — stays literal text, so "fish & chips" needs no escaping.
 * Decoded text is a plain string escaped at render time, so "&amp;" in source
 * round-trips to "&amp;" in output. Text decodes after whitespace
 * normalization, so "&#32;" survives it (the entity spelling of JSX's {' '}). */

static const zend_markup_entity *zend_markup_entity_lookup(const char *name, size_t len)
{
	uint32_t lo = 0, hi = ZEND_MARKUP_ENTITY_COUNT;

	while (lo < hi) {
		uint32_t mid = lo + (hi - lo) / 2;
		const zend_markup_entity *e = &zend_markup_entities[mid];
		int r = memcmp(name, e->name, MIN(len, e->name_len));
		if (r == 0) {
			r = (len > e->name_len) - (len < e->name_len);
		}
		if (r == 0) {
			return e;
		}
		if (r < 0) {
			hi = mid;
		} else {
			lo = mid + 1;
		}
	}
	return NULL;
}

static size_t zend_markup_utf8_encode(char *out, uint32_t cp)
{
	if (cp < 0x80) {
		out[0] = (char)cp;
		return 1;
	}
	if (cp < 0x800) {
		out[0] = (char)(0xC0 | (cp >> 6));
		out[1] = (char)(0x80 | (cp & 0x3F));
		return 2;
	}
	if (cp < 0x10000) {
		out[0] = (char)(0xE0 | (cp >> 12));
		out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
		out[2] = (char)(0x80 | (cp & 0x3F));
		return 3;
	}
	out[0] = (char)(0xF0 | (cp >> 18));
	out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
	out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
	out[3] = (char)(0x80 | (cp & 0x3F));
	return 4;
}

/* Takes ownership of str; returns it unchanged when it contains no "&". */
zend_string *zend_markup_decode_entities(zend_string *str)
{
	const char *p = ZSTR_VAL(str);
	const char *end = p + ZSTR_LEN(str);
	const char *amp = memchr(p, '&', ZSTR_LEN(str));
	smart_str buf = {0};

	if (!amp) {
		return str;
	}

	while (amp) {
		const char *q = amp + 1;
		const char *repl = NULL;
		size_t repl_len = 0;
		char utf8[4];

		smart_str_appendl(&buf, p, amp - p);

		if (q < end && *q == '#') {
			/* Numeric reference: &#123; or &#x1F600; */
			uint32_t cp = 0;
			bool overflow = false, hex = false;
			const char *digits;

			q++;
			if (q < end && (*q == 'x' || *q == 'X')) {
				hex = true;
				q++;
			}
			digits = q;
			while (q < end) {
				uint32_t d;
				if (*q >= '0' && *q <= '9') {
					d = *q - '0';
				} else if (hex && *q >= 'a' && *q <= 'f') {
					d = *q - 'a' + 10;
				} else if (hex && *q >= 'A' && *q <= 'F') {
					d = *q - 'A' + 10;
				} else {
					break;
				}
				if (cp > 0x10FFFF) {
					overflow = true;
				} else {
					cp = cp * (hex ? 16 : 10) + d;
				}
				q++;
			}
			if (q > digits && q < end && *q == ';'
					&& !overflow && cp > 0 && cp <= 0x10FFFF
					&& !(cp >= 0xD800 && cp <= 0xDFFF)) {
				repl_len = zend_markup_utf8_encode(utf8, cp);
				repl = utf8;
				q++;
			}
		} else {
			/* Named reference: &name; (case-sensitive, ASCII alphanumeric) */
			const char *name = q;
			while (q < end && (size_t)(q - name) <= ZEND_MARKUP_ENTITY_MAX_NAME
					&& ((*q >= 'a' && *q <= 'z') || (*q >= 'A' && *q <= 'Z')
						|| (*q >= '0' && *q <= '9'))) {
				q++;
			}
			if (q > name && q < end && *q == ';') {
				const zend_markup_entity *e = zend_markup_entity_lookup(name, q - name);
				if (e) {
					repl = e->utf8;
					repl_len = e->utf8_len;
					q++;
				}
			}
		}

		if (repl) {
			smart_str_appendl(&buf, repl, repl_len);
			p = q;
		} else {
			smart_str_appendc(&buf, '&');
			p = amp + 1;
		}
		amp = (p < end) ? memchr(p, '&', end - p) : NULL;
	}
	smart_str_appendl(&buf, p, end - p);

	zend_string_release_ex(str, 0);
	return smart_str_extract(&buf);
}

/* Native markup AST lowering (RFC: Native Markup Expressions, Phase 2/3).
 * Lower a markup element into an ordinary `new \Html\Element(tag, [], [children])`
 * (or `new \Html\Fragment([children])` when the tag name is empty) so no new AST
 * node kind or zend_compile.c support is needed. `name` is the tag-name zval AST
 * or NULL for a fragment; `children` is a ZEND_AST_ARRAY of ZEND_AST_ARRAY_ELEM.
 *
 * All lowering targets are named through the ZEND_MARKUP_*_FQ macros (see
 * zend_markup.h) so the markup-to-runtime vocabulary has a single home. */

/* Build a fully-qualified name AST for one of the ZEND_MARKUP_*_FQ symbols. */
static zend_ast *zend_ast_markup_fq_name(const char *name, size_t len)
{
	zend_ast *ast = zend_ast_create_zval_from_str(zend_string_init(name, len, 0));
	ast->attr = ZEND_NAME_FQ;
	return ast;
}

/* Element vs component classification (RFC §4): a tag whose first character is
 * uppercase, which is namespace-qualified (contains "\"), or which names a static
 * method via "::", dispatches as a component; anything else is a literal HTML
 * element. The single home for this rule: the compiler applies it to static tag
 * names here, and Html\render_dynamic applies it to a dynamic tag's runtime
 * value (hence ZEND_API). */
ZEND_API bool zend_markup_name_is_component(zend_string *tag)
{
	return (ZSTR_LEN(tag) > 0 && ZSTR_VAL(tag)[0] >= 'A' && ZSTR_VAL(tag)[0] <= 'Z')
		|| memchr(ZSTR_VAL(tag), '\\', ZSTR_LEN(tag)) != NULL
		|| strstr(ZSTR_VAL(tag), "::") != NULL;
}

/* Wrap a list of children in `new \Html\Fragment([children])`. Consumes (and, if
 * NULL, creates) the children list. */
static zend_ast *zend_ast_wrap_fragment(zend_ast *children)
{
	if (children == NULL) {
		children = zend_ast_create_list(0, ZEND_AST_ARRAY);
	}
	children->attr = ZEND_ARRAY_SYNTAX_SHORT;

	zend_ast *frag_class = zend_ast_markup_fq_name(ZEND_STRL(ZEND_MARKUP_FRAGMENT_FQ));

	return zend_ast_create(ZEND_AST_NEW, frag_class,
		zend_ast_create_list(1, ZEND_AST_ARG_LIST, children));
}

/* Wrap a string AST in a `\Html\raw(...)` call (trusted passthrough). Used for
 * `<!-- … -->` comments, which are emitted as literal HTML (RFC §7). */
zend_ast *zend_ast_create_markup_raw(zend_ast *str)
{
	zend_ast *fn = zend_ast_markup_fq_name(ZEND_STRL(ZEND_MARKUP_RAW_FQ));
	return zend_ast_create(ZEND_AST_CALL, fn,
		zend_ast_create_list(1, ZEND_AST_ARG_LIST, str));
}

/* Build a `<slot:name>…</slot:name>` block (RFC §6) as a *keyed* array element
 * `'name' => new \Html\Fragment([children])`. The component builder later routes
 * keyed elements to render_component's namedSlots argument. `close` is consumed. */
zend_ast *zend_ast_create_markup_slot(zend_ast *open, zend_ast *attrs, zend_ast *children, zend_ast *close)
{
	if (close != NULL) {
		if (!zend_string_equals(Z_STR_P(zend_ast_get_zval(open)), Z_STR_P(zend_ast_get_zval(close)))) {
			zend_throw_exception_ex(zend_ce_compile_error, 0,
				"Mismatched markup closing tag: expected </slot:%s>, found </slot:%s>",
				Z_STRVAL_P(zend_ast_get_zval(open)), Z_STRVAL_P(zend_ast_get_zval(close)));
			zend_ast_destroy(open);
			zend_ast_destroy(attrs);
			if (children != NULL) {
				zend_ast_destroy(children);
			}
			zend_ast_destroy(close);
			return NULL;
		}
		zend_ast_destroy(close);
	}
	/* A slot block routes body content only; silently discarding written
	 * attributes would hide a mistake. */
	if (attrs != NULL && zend_ast_get_list(attrs)->children > 0) {
		zend_throw_exception_ex(zend_ce_compile_error, 0,
			"Markup slot <slot:%s> cannot have attributes",
			Z_STRVAL_P(zend_ast_get_zval(open)));
		zend_ast_destroy(open);
		zend_ast_destroy(attrs);
		if (children != NULL) {
			zend_ast_destroy(children);
		}
		return NULL;
	}
	if (attrs != NULL) {
		zend_ast_destroy(attrs);
	}
	return zend_ast_create(ZEND_AST_ARRAY_ELEM, zend_ast_wrap_fragment(children), open);
}

/* Partition a markup body into loose content and <slot:name> blocks: keyed
 * elements are named slots, everything else forms the anonymous slot (RFC §6).
 * Consumes `children` (may be NULL); both outputs are fresh ZEND_AST_ARRAY
 * lists with ZEND_ARRAY_SYNTAX_SHORT set. */
static void zend_markup_partition_children(zend_ast *children, zend_ast **anon_out, zend_ast **named_out)
{
	zend_ast *anon = zend_ast_create_list(0, ZEND_AST_ARRAY);
	zend_ast *named_slots = zend_ast_create_list(0, ZEND_AST_ARRAY);

	if (children != NULL) {
		zend_ast_list *clist = zend_ast_get_list(children);
		for (uint32_t i = 0; i < clist->children; i++) {
			zend_ast *elem = clist->child[i];
			if (elem != NULL && elem->kind == ZEND_AST_ARRAY_ELEM && elem->child[1] != NULL) {
				named_slots = zend_ast_list_add(named_slots, elem);
			} else {
				anon = zend_ast_list_add(anon, elem);
			}
		}
		/* Detach the moved elements before freeing the now-empty container. */
		clist->children = 0;
		zend_ast_destroy(children);
	}
	anon->attr = ZEND_ARRAY_SYNTAX_SHORT;
	named_slots->attr = ZEND_ARRAY_SYNTAX_SHORT;

	*anon_out = anon;
	*named_out = named_slots;
}

/* Lower a component tag (capitalized name, namespace-qualified name, or
 * "Class::method") into a call to
 * \Html\render_component(component, props, slot, namedSlots, functionComponent)
 * (RFC §3). Attributes become the props array; loose body content becomes a
 * single \Html\Fragment passed as the anonymous slot, and <slot:name> blocks
 * (keyed elements) are partitioned into the namedSlots array.
 *
 * Component resolution is two-stage. A bare tag could name either a class
 * component or a function component, which PHP resolves through *separate* import
 * tables (`use` vs `use function`). So this pass resolves the name *both* ways —
 * a class candidate (ZEND_AST_CLASS_NAME, the `component` argument) and a function
 * candidate (ZEND_AST_CALLABLE_NAME, the `functionComponent` argument) — and the
 * runtime applies the dispatch order (Class::method, then Htmlable class, then
 * userland function) across the two, so `use App\C;` resolves a class and
 * `use function App\C;` resolves a function, exactly as ordinary PHP does. A
 * leading "\" makes both candidates fully qualified. A "Class::method" tag is
 * unambiguously a static method, so it has no function candidate. See
 * PHP_FUNCTION(Html_render_component) in ext/html/html.c for the second stage. */
static zend_ast *zend_ast_create_markup_component(zend_ast *name, zend_ast *attrs, zend_ast *children)
{
	zend_string *tag = zend_ast_get_str(name);
	const char *sep = strstr(ZSTR_VAL(tag), "::");
	/* A leading "\" is a fully-qualified reference; otherwise resolve the name
	 * (unqualified or qualified) against the current use/namespace. */
	uint32_t name_type = (ZSTR_LEN(tag) > 0 && ZSTR_VAL(tag)[0] == '\\')
		? ZEND_NAME_FQ : ZEND_NAME_NOT_FQ;
	zend_ast *component_name;
	zend_ast *function_name;

	if (sep != NULL) {
		/* "Class::method": resolve the class part as a class, then append
		 * "::method". A static method is never a plain function. */
		size_t class_len = sep - ZSTR_VAL(tag);
		zend_ast *class_name = zend_ast_create_zval_from_str(
			zend_string_init(ZSTR_VAL(tag), class_len, 0));
		class_name->attr = name_type;

		zval suffix;
		ZVAL_STR(&suffix, zend_string_init(sep, ZSTR_LEN(tag) - class_len, 0));
		component_name = zend_ast_create_concat_op(
			zend_ast_create(ZEND_AST_CLASS_NAME, class_name),
			zend_ast_create_zval(&suffix));

		zval null_zv;
		ZVAL_NULL(&null_zv);
		function_name = zend_ast_create_zval(&null_zv);
	} else {
		/* Resolve the name both as a class (honors `use`) and as a function
		 * (honors `use function`); the runtime picks between them. */
		zend_ast *class_name = zend_ast_create_zval_from_str(zend_string_copy(tag));
		class_name->attr = name_type;
		component_name = zend_ast_create(ZEND_AST_CLASS_NAME, class_name);

		zend_ast *func_name = zend_ast_create_zval_from_str(zend_string_copy(tag));
		func_name->attr = name_type;
		function_name = zend_ast_create(ZEND_AST_CALLABLE_NAME, func_name);
	}
	zend_ast_destroy(name);

	if (attrs == NULL) {
		attrs = zend_ast_create_list(0, ZEND_AST_ARRAY);
	}
	attrs->attr = ZEND_ARRAY_SYNTAX_SHORT;

	/* Partition the body: keyed elements are <slot:name> blocks (named slots);
	 * everything else is loose content forming the anonymous slot (RFC §6). */
	zend_ast *anon, *named_slots;
	zend_markup_partition_children(children, &anon, &named_slots);

	/* Anonymous slot: a single Html\Fragment, or null when there is no loose body. */
	zend_ast *slot;
	if (zend_ast_get_list(anon)->children > 0) {
		slot = zend_ast_wrap_fragment(anon);
	} else {
		zend_ast_destroy(anon);
		zval null_zv;
		ZVAL_NULL(&null_zv);
		slot = zend_ast_create_zval(&null_zv);
	}

	zend_ast *fn = zend_ast_markup_fq_name(ZEND_STRL(ZEND_MARKUP_COMPONENT_FQ));

	zend_ast *args = zend_ast_create_list(0, ZEND_AST_ARG_LIST);
	args = zend_ast_list_add(args, component_name);
	args = zend_ast_list_add(args, attrs);
	args = zend_ast_list_add(args, slot);
	args = zend_ast_list_add(args, named_slots);
	args = zend_ast_list_add(args, function_name);

	return zend_ast_create(ZEND_AST_CALL, fn, args);
}

zend_ast *zend_ast_create_markup_element(zend_ast *name, zend_ast *attrs, zend_ast *children)
{
	zend_ast *class_name, *args, *result;

	/* The grammar reduces a markup element only once its closing tag has been
	 * scanned, so nodes created here would inherit the *close* line. Stamp the
	 * lowered expression with the line the element opens on instead, so errors
	 * in a multi-line element report like any multi-line call. The tag-name
	 * token carries that line; a fragment has none, but its children list is
	 * created as content begins. */
	uint32_t lineno = (name != NULL)
		? zend_ast_get_lineno(name) : zend_ast_get_lineno(children);

	/* A capitalized name, or one containing "::", is a component (RFC §3). */
	if (name != NULL && zend_markup_name_is_component(zend_ast_get_str(name))) {
		result = zend_ast_create_markup_component(name, attrs, children);
		result->lineno = lineno;
		return result;
	}

	/* <slot:name> blocks lower to keyed children and are only routed by the
	 * component builder above; in a plain element or fragment one would slip
	 * into the children array as a stray string key. */
	zend_ast_list *clist = zend_ast_get_list(children);
	for (uint32_t i = 0; i < clist->children; i++) {
		zend_ast *elem = clist->child[i];
		if (elem != NULL && elem->kind == ZEND_AST_ARRAY_ELEM && elem->child[1] != NULL) {
			zend_throw_exception_ex(zend_ce_compile_error, 0,
				"Markup slot <slot:%s> is only allowed in a component body",
				Z_STRVAL_P(zend_ast_get_zval(elem->child[1])));
			if (name != NULL) {
				zend_ast_destroy(name);
			}
			if (attrs != NULL) {
				zend_ast_destroy(attrs);
			}
			zend_ast_destroy(children);
			return NULL;
		}
	}

	children->attr = ZEND_ARRAY_SYNTAX_SHORT;

	args = zend_ast_create_list(0, ZEND_AST_ARG_LIST);

	if (name == NULL) {
		/* A fragment has no attributes (the grammar forbids them on `<>`). */
		if (attrs != NULL) {
			zend_ast_destroy(attrs);
		}
		class_name = zend_ast_markup_fq_name(ZEND_STRL(ZEND_MARKUP_FRAGMENT_FQ));
		args = zend_ast_list_add(args, children);
	} else {
		if (attrs == NULL) {
			attrs = zend_ast_create_list(0, ZEND_AST_ARRAY);
		}
		attrs->attr = ZEND_ARRAY_SYNTAX_SHORT;
		class_name = zend_ast_markup_fq_name(ZEND_STRL(ZEND_MARKUP_ELEMENT_FQ));
		args = zend_ast_list_add(args, name);
		args = zend_ast_list_add(args, attrs);
		args = zend_ast_list_add(args, children);
	}

	result = zend_ast_create(ZEND_AST_NEW, class_name, args);
	result->lineno = lineno;
	return result;
}

/* Build a markup element after checking the closing tag matches the opener
 * (RFC §7). Both NULL means a `<>…</>` fragment. `close` is consumed. */
zend_ast *zend_ast_create_markup_checked(zend_ast *open, zend_ast *attrs, zend_ast *children, zend_ast *close)
{
	bool match;
	if (open == NULL || close == NULL) {
		match = (open == NULL && close == NULL);
	} else {
		match = zend_string_equals(Z_STR_P(zend_ast_get_zval(open)), Z_STR_P(zend_ast_get_zval(close)));
	}
	if (!match) {
		zend_throw_exception_ex(zend_ce_compile_error, 0,
			"Mismatched markup closing tag: expected </%s>, found </%s>",
			open ? Z_STRVAL_P(zend_ast_get_zval(open)) : "",
			close ? Z_STRVAL_P(zend_ast_get_zval(close)) : "");
		goto fail;
	}
	if (close != NULL) {
		zend_ast_destroy(close);
		close = NULL;
	}

	return zend_ast_create_markup_element(open, attrs, children);

fail:
	if (open != NULL) {
		zend_ast_destroy(open);
	}
	if (attrs != NULL) {
		zend_ast_destroy(attrs);
	}
	zend_ast_destroy(children);
	if (close != NULL) {
		zend_ast_destroy(close);
	}
	return NULL;
}

/* Lower a dynamic tag `<$tag …>…</$tag>` (RFC §4) into a call to
 * \Html\render_dynamic($tag, attributes, children, namedSlots). The variable's
 * runtime value decides what a static tag name decides at compile time, by the
 * same classification rule (zend_markup_name_is_component): a component name
 * dispatches through render_component's machinery (children become the
 * anonymous-slot Fragment, hooks and decorators included), anything else
 * constructs a literal \Html\Element. Only classification moves to runtime;
 * the body is still partitioned into loose children vs <slot:name> blocks
 * here, at compile time. `name` is the T_VARIABLE zval AST (the name without
 * the "$"). */
/* Build the \Html\render_dynamic(tag_expr, attributes, children, namedSlots)
 * call both dynamic-tag forms lower into. `tag_expr` is an already-built
 * expression AST producing the tag name; `lineno` is the opening-tag line
 * (see zend_ast_create_markup_element for why it is stamped explicitly). */
static zend_ast *zend_markup_dynamic_call(zend_ast *tag_expr, zend_ast *attrs, zend_ast *children, uint32_t lineno)
{
	if (attrs == NULL) {
		attrs = zend_ast_create_list(0, ZEND_AST_ARRAY);
	}
	attrs->attr = ZEND_ARRAY_SYNTAX_SHORT;

	zend_ast *anon, *named_slots;
	zend_markup_partition_children(children, &anon, &named_slots);

	zend_ast *fn = zend_ast_markup_fq_name(ZEND_STRL(ZEND_MARKUP_DYNAMIC_FQ));

	zend_ast *args = zend_ast_create_list(0, ZEND_AST_ARG_LIST);
	args = zend_ast_list_add(args, tag_expr);
	args = zend_ast_list_add(args, attrs);
	args = zend_ast_list_add(args, anon);
	args = zend_ast_list_add(args, named_slots);

	zend_ast *result = zend_ast_create(ZEND_AST_CALL, fn, args);
	result->lineno = lineno;
	return result;
}

zend_ast *zend_ast_create_markup_dynamic(zend_ast *name, zend_ast *attrs, zend_ast *children)
{
	uint32_t lineno = zend_ast_get_lineno(name);

	zend_ast *var = zend_ast_create(ZEND_AST_VAR, name);
	var->lineno = lineno;

	return zend_markup_dynamic_call(var, attrs, children, lineno);
}

/* The brace form `<{expr} …>…</>` / `<{expr} …/>`: the tag is an arbitrary
 * expression, evaluated once. It closes anonymously (`</>`, like a fragment)
 * because a closing tag cannot restate the expression - re-evaluating it
 * would run its side effects twice, and matching it textually is meaningless
 * for computed values. */
zend_ast *zend_ast_create_markup_dynamic_expr(zend_ast *expr, zend_ast *attrs, zend_ast *children)
{
	return zend_markup_dynamic_call(expr, attrs, children, zend_ast_get_lineno(expr));
}

/* Build a dynamic tag after checking the closing variable matches the opener:
 * `<$a>…</$b>` is a compile error, exactly as `<div>…</span>` is. `close` is
 * consumed. */
zend_ast *zend_ast_create_markup_dynamic_checked(zend_ast *open, zend_ast *attrs, zend_ast *children, zend_ast *close)
{
	if (!zend_string_equals(Z_STR_P(zend_ast_get_zval(open)), Z_STR_P(zend_ast_get_zval(close)))) {
		zend_throw_exception_ex(zend_ce_compile_error, 0,
			"Mismatched markup closing tag: expected </$%s>, found </$%s>",
			Z_STRVAL_P(zend_ast_get_zval(open)), Z_STRVAL_P(zend_ast_get_zval(close)));
		zend_ast_destroy(open);
		if (attrs != NULL) {
			zend_ast_destroy(attrs);
		}
		zend_ast_destroy(children);
		zend_ast_destroy(close);
		return NULL;
	}
	zend_ast_destroy(close);

	return zend_ast_create_markup_dynamic(open, attrs, children);
}
