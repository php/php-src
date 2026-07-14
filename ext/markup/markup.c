/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the world-wide-web at the following url:                     |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Liam Hammett                                                 |
   +----------------------------------------------------------------------+
*/

/* Native Markup Expressions.
 *
 * This extension provides the `Markup\` value-object tree that markup expressions
 * lower into: Element, Fragment, Raw, the Html interface, the Slot
 * attribute, and the raw()/escape() helpers. It owns escaping and serialization
 * so the escape-by-default safety guarantee lives in core, not userland.
 *
 * No parser/scanner changes live here; the markup syntax that lowers into these
 * symbols lives in Zend/zend_language_scanner.l, Zend/zend_language_parser.y,
 * and Zend/zend_markup.c (see Zend/zend_markup.h for the shared vocabulary). */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_markup.h"

#include "Zend/zend_attributes.h"
#include "Zend/zend_closures.h"
#include "Zend/zend_markup.h"
#include "Zend/zend_extensions.h"
#include "Zend/zend_interfaces.h"
#include "Zend/zend_iterators.h"
#include "Zend/zend_observer.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_language_scanner.h"
#include "ext/standard/html.h"
#include "ext/standard/info.h"

#include "markup_arginfo.h"

ZEND_DECLARE_MODULE_GLOBALS(markup)

zend_class_entry *markup_ce_Html;
zend_class_entry *markup_ce_Element;
zend_class_entry *markup_ce_Fragment;
zend_class_entry *markup_ce_Raw;
zend_class_entry *markup_ce_Slot;
zend_class_entry *markup_ce_LazyFragment;

/* Markup\LazyFragment carries internal state a plain property bag cannot: a
 * Closure thunk plus a memoized result that is written on first render. A
 * custom object holds both out of userland view; the class exposes no
 * properties. */
typedef struct {
	zval thunk;     /* the Closure building the deferred body */
	zval resolved;  /* memoized Markup\Html, IS_UNDEF until first render */
	zend_object std;
} markup_lazy_fragment;

static zend_object_handlers markup_lazy_fragment_handlers;

static zend_always_inline markup_lazy_fragment *markup_lazy_fragment_from_obj(zend_object *obj)
{
	return (markup_lazy_fragment *)((char *)obj - offsetof(markup_lazy_fragment, std));
}

static zend_object *markup_lazy_fragment_create(zend_class_entry *ce)
{
	markup_lazy_fragment *lf = zend_object_alloc(sizeof(markup_lazy_fragment), ce);
	zend_object_std_init(&lf->std, ce);
	object_properties_init(&lf->std, ce);
	ZVAL_UNDEF(&lf->thunk);
	ZVAL_UNDEF(&lf->resolved);
	lf->std.handlers = &markup_lazy_fragment_handlers;
	return &lf->std;
}

static void markup_lazy_fragment_free(zend_object *obj)
{
	markup_lazy_fragment *lf = markup_lazy_fragment_from_obj(obj);
	zval_ptr_dtor(&lf->thunk);
	zval_ptr_dtor(&lf->resolved);
	zend_object_std_dtor(&lf->std);
}

static HashTable *markup_lazy_fragment_get_gc(zend_object *obj, zval **table, int *n)
{
	markup_lazy_fragment *lf = markup_lazy_fragment_from_obj(obj);
	zend_get_gc_buffer *buf = zend_get_gc_buffer_create();
	zend_get_gc_buffer_add_zval(buf, &lf->thunk);
	zend_get_gc_buffer_add_zval(buf, &lf->resolved);
	zend_get_gc_buffer_use(buf, table, n);
	return zend_std_get_properties(obj);
}

/* Method-table names of the default __toString() injected into userland
 * Html classes (see markup_implement_html). Permanent interned. */
static zend_string *markup_str_tostring;    /* "__toString" */
static zend_string *markup_str_tostring_lc; /* "__tostring" */

/* arg_info for the injected default __toString(). The raw generated arginfo
 * uses the registration-time zend_internal_arg_info layout; a function living
 * in a class table must use the converted zend_arg_info layout (reflection and
 * the VM assume it). Converted once at MINIT, exactly like zend_enum.c's
 * zarginfo_* arrays for the injected cases()/from()/tryFrom(). */
static zend_arg_info markup_zarginfo_default_tostring[
	sizeof(arginfo_class_Markup_Html___toString) / sizeof(zend_internal_arg_info)];

static zend_string *markup_render_html(zval *value);

/* htmlspecialchars($s, ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401). The charset
 * hint is NULL so the ini default_charset applies (UTF-8 by default), exactly as
 * htmlspecialchars() does - a non-UTF-8 application keeps working. */
static zend_string *markup_escape_string(const char *s, size_t len)
{
	return php_escape_html_entities(
		(const unsigned char *) s, len, /* all */ 0,
		ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML401, NULL);
}

static void markup_append_escaped(smart_str *buf, zend_string *str)
{
	zend_string *escaped = markup_escape_string(ZSTR_VAL(str), ZSTR_LEN(str));
	smart_str_append(buf, escaped);
	zend_string_release(escaped);
}

/* A tag name must start with a letter and contain only name-safe characters, so a
 * dynamically-supplied tag (`new Element($x)`) can never break out of the markup. */
static bool markup_is_valid_tag_name(zend_string *tag)
{
	if (ZSTR_LEN(tag) == 0) {
		return false;
	}
	const char *p = ZSTR_VAL(tag);
	if (!((p[0] >= 'a' && p[0] <= 'z') || (p[0] >= 'A' && p[0] <= 'Z'))) {
		return false;
	}
	for (size_t i = 1; i < ZSTR_LEN(tag); i++) {
		char c = p[i];
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
				|| (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == ':')) {
			return false;
		}
	}
	return true;
}

/* An attribute name may not contain characters that would let a dynamically
 * supplied key (spread, `new Element(...)`) break out of the attribute (HTML5
 * attribute-name production). */
static bool markup_is_valid_attr_name(zend_string *name)
{
	if (ZSTR_LEN(name) == 0) {
		return false;
	}
	for (size_t i = 0; i < ZSTR_LEN(name); i++) {
		unsigned char c = (unsigned char) ZSTR_VAL(name)[i];
		if (c < 0x20 || c == 0x7f || c == ' ' || c == '"' || c == '\''
				|| c == '>' || c == '/' || c == '=' || c == '<') {
			return false;
		}
	}
	return true;
}

/* HTML5 void elements: serialized without a closing tag or trailing slash. */
static bool markup_is_void_element(zend_string *tag)
{
	static const char *const void_elements[] = {
		"area", "base", "br", "col", "embed", "hr", "img", "input",
		"link", "meta", "param", "source", "track", "wbr", NULL,
	};

	for (const char *const *v = void_elements; *v != NULL; v++) {
		size_t len = strlen(*v);
		if (ZSTR_LEN(tag) == len
				&& zend_binary_strcasecmp(ZSTR_VAL(tag), len, *v, len) == 0) {
			return true;
		}
	}
	return false;
}

/* Bound the pure-C child recursion so a deeply nested array/Traversable cannot
 * overflow the stack. HTML this deep is absurd; the limit only stops abuse. */
#define MARKUP_MAX_DEPTH 1024

static zend_result markup_append_child(smart_str *buf, zval *value, uint32_t depth);

/* Flatten a Traversable, coercing each yielded value recursively.
 * Keys are ignored; only values render. */
static zend_result markup_append_traversable(smart_str *buf, zval *value, uint32_t depth)
{
	zend_class_entry *ce = Z_OBJCE_P(value);
	zend_object_iterator *iter = ce->get_iterator(ce, value, /* by_ref */ 0);
	zend_result result = SUCCESS;

	if (iter == NULL || EG(exception)) {
		return FAILURE;
	}

	if (iter->funcs->rewind) {
		iter->funcs->rewind(iter);
		if (EG(exception)) {
			result = FAILURE;
			goto done;
		}
	}

	while (iter->funcs->valid(iter) == SUCCESS) {
		if (EG(exception)) {
			result = FAILURE;
			break;
		}

		zval *element = iter->funcs->get_current_data(iter);
		if (EG(exception) || element == NULL) {
			result = FAILURE;
			break;
		}

		if (markup_append_child(buf, element, depth + 1) == FAILURE) {
			result = FAILURE;
			break;
		}

		iter->funcs->move_forward(iter);
		if (EG(exception)) {
			result = FAILURE;
			break;
		}
	}

done:
	zend_iterator_dtor(iter);
	return EG(exception) ? FAILURE : result;
}

/* Coerce and append a single child value.
 * Returns FAILURE with an exception pending on an unrenderable value. */
static zend_result markup_append_child(smart_str *buf, zval *value, uint32_t depth)
{
	if (depth > MARKUP_MAX_DEPTH) {
		zend_throw_error(NULL, "Maximum markup nesting depth of %d exceeded", MARKUP_MAX_DEPTH);
		return FAILURE;
	}

	switch (Z_TYPE_P(value)) {
		case IS_NULL:
		case IS_FALSE:
			/* renders as "" */
			return SUCCESS;

		case IS_TRUE:
			/* (string) true === "1" */
			smart_str_appendc(buf, '1');
			return SUCCESS;

		case IS_LONG:
			smart_str_append_long(buf, Z_LVAL_P(value));
			return SUCCESS;

		case IS_DOUBLE: {
			zend_string *s = zend_double_to_str(Z_DVAL_P(value));
			smart_str_append(buf, s);
			zend_string_release(s);
			return SUCCESS;
		}

		case IS_STRING:
			markup_append_escaped(buf, Z_STR_P(value));
			return SUCCESS;

		case IS_ARRAY: {
			/* flatten, coercing each element recursively */
			zval *element;
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(value), element) {
				ZVAL_DEREF(element);
				if (markup_append_child(buf, element, depth + 1) == FAILURE) {
					return FAILURE;
				}
			} ZEND_HASH_FOREACH_END();
			return SUCCESS;
		}

		case IS_OBJECT: {
			zend_class_entry *ce = Z_OBJCE_P(value);

			/* Html: already-safe HTML, passes through un-escaped. Resolved
			 * through toHtml() to one of the internal node classes and then
			 * serialized natively - never through __toString, so overriding a
			 * component's string form cannot change what its markup renders as. */
			if (instanceof_function(ce, markup_ce_Html)) {
				zend_string *s = markup_render_html(value);
				if (!s) {
					return FAILURE;
				}
				smart_str_append(buf, s);
				zend_string_release(s);
				return SUCCESS;
			}

			/* Traversable: flatten, coercing each yielded value recursively. */
			if (instanceof_function(ce, zend_ce_traversable)) {
				return markup_append_traversable(buf, value, depth);
			}

			/* Stringable: cast then escape like a string. */
			if (instanceof_function(ce, zend_ce_stringable)) {
				zend_string *s = zval_try_get_string(value);
				if (!s) {
					return FAILURE;
				}
				markup_append_escaped(buf, s);
				zend_string_release(s);
				return SUCCESS;
			}

			zend_type_error(
				"Object of class %s cannot be rendered as a markup child: "
				"it implements none of Markup\\Html, Stringable, or Traversable",
				ZSTR_VAL(ce->name));
			return FAILURE;
		}

		case IS_REFERENCE:
			return markup_append_child(buf, Z_REFVAL_P(value), depth);

		default:
			zend_type_error(
				"Value of type %s cannot be rendered as a markup child",
				zend_zval_value_name(value));
			return FAILURE;
	}
}

/* Emit ` name="value"` into the buffer and release `value`. The `escape` flag
 * is the one security-relevant decision in attribute emission: every value is
 * escaped except an Html's already-safe HTML (and numbers, which cannot
 * contain escapable characters). Values are always serialized double-quoted -
 * no unquoted output form exists. */
static void markup_append_attr_value(smart_str *buf, zend_string *name, zend_string *value, bool escape)
{
	smart_str_appendc(buf, ' ');
	smart_str_append(buf, name);
	smart_str_appendl(buf, "=\"", 2);
	if (escape) {
		markup_append_escaped(buf, value);
	} else {
		smart_str_append(buf, value);
	}
	smart_str_appendc(buf, '"');
	zend_string_release(value);
}

/* Coerce and append a single attribute value. */
static zend_result markup_append_attribute(smart_str *buf, zend_string *name, zval *value)
{
	switch (Z_TYPE_P(value)) {
		case IS_NULL:
		case IS_FALSE:
			/* attribute omitted */
			return SUCCESS;

		case IS_TRUE:
			/* bare boolean attribute */
			smart_str_appendc(buf, ' ');
			smart_str_append(buf, name);
			return SUCCESS;

		case IS_LONG:
			markup_append_attr_value(buf, name, zend_long_to_str(Z_LVAL_P(value)), false);
			return SUCCESS;

		case IS_DOUBLE:
			markup_append_attr_value(buf, name, zend_double_to_str(Z_DVAL_P(value)), false);
			return SUCCESS;

		case IS_STRING:
			markup_append_attr_value(buf, name, zend_string_copy(Z_STR_P(value)), true);
			return SUCCESS;

		case IS_OBJECT: {
			zend_class_entry *value_ce = Z_OBJCE_P(value);

			/* Html (Markup\raw(), Markup\escape(), ...) is already-safe HTML and
			 * passes through un-escaped, exactly as in child position - so
			 * title={\Markup\raw('&nbsp;')} emits the entity rather than
			 * double-escaping it. The trust contract is the caller's: a raw
			 * value containing '"' can break out of the attribute. */
			if (instanceof_function(value_ce, markup_ce_Html)) {
				zend_string *s = markup_render_html(value);
				if (!s) {
					return FAILURE;
				}
				markup_append_attr_value(buf, name, s, false);
				return SUCCESS;
			}

			if (instanceof_function(value_ce, zend_ce_stringable)) {
				zend_string *s = zval_try_get_string(value);
				if (!s) {
					return FAILURE;
				}
				markup_append_attr_value(buf, name, s, true);
				return SUCCESS;
			}
			ZEND_FALLTHROUGH;
		}

		default:
			/* arrays are reserved for a future class/style helper */
			zend_type_error(
				"Attribute \"%s\" value must be of type string|int|float|bool|null|Stringable, %s given",
				ZSTR_VAL(name), zend_zval_value_name(value));
			return FAILURE;
	}
}

static zend_result markup_append_attributes(smart_str *buf, HashTable *attributes)
{
	zend_string *name;
	zval *value;

	ZEND_HASH_FOREACH_STR_KEY_VAL(attributes, name, value) {
		if (name == NULL) {
			zend_type_error(
				"Attribute names must be strings, integer given");
			return FAILURE;
		}
		if (!markup_is_valid_attr_name(name)) {
			zend_value_error("Invalid attribute name \"%s\"", ZSTR_VAL(name));
			return FAILURE;
		}
		ZVAL_DEREF(value);
		if (markup_append_attribute(buf, name, value) == FAILURE) {
			return FAILURE;
		}
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

/* Render a Markup\Element to its smart_str buffer. */
static zend_result markup_render_element(smart_str *buf, zend_object *obj)
{
	zval rv_tag, rv_attrs, rv_children;
	zval *tag = zend_read_property(markup_ce_Element, obj, ZEND_STRL("tag"), /* silent */ true, &rv_tag);
	zval *attributes = zend_read_property(markup_ce_Element, obj, ZEND_STRL("attributes"), true, &rv_attrs);
	zval *children = zend_read_property(markup_ce_Element, obj, ZEND_STRL("children"), true, &rv_children);

	ZVAL_DEREF(tag);
	ZVAL_DEREF(attributes);
	ZVAL_DEREF(children);

	if (Z_TYPE_P(tag) != IS_STRING || Z_TYPE_P(attributes) != IS_ARRAY || Z_TYPE_P(children) != IS_ARRAY) {
		zend_throw_error(NULL, "Markup\\Element has not been initialized");
		return FAILURE;
	}

	if (!markup_is_valid_tag_name(Z_STR_P(tag))) {
		zend_value_error("Invalid tag name \"%s\"", ZSTR_VAL(Z_STR_P(tag)));
		return FAILURE;
	}

	smart_str_appendc(buf, '<');
	smart_str_append(buf, Z_STR_P(tag));

	if (markup_append_attributes(buf, Z_ARRVAL_P(attributes)) == FAILURE) {
		return FAILURE;
	}

	if (markup_is_void_element(Z_STR_P(tag))) {
		/* Clean HTML5 void element: no slash, no closing tag. Children would be
		 * silently lost in serialization, so reject them outright. */
		if (zend_hash_num_elements(Z_ARRVAL_P(children)) > 0) {
			zend_throw_error(NULL,
				"Void element <%s> cannot have children", ZSTR_VAL(Z_STR_P(tag)));
			return FAILURE;
		}
		smart_str_appendc(buf, '>');
		return SUCCESS;
	}

	smart_str_appendc(buf, '>');

	if (markup_append_child(buf, children, 0) == FAILURE) {
		return FAILURE;
	}

	smart_str_appendl(buf, "</", 2);
	smart_str_append(buf, Z_STR_P(tag));
	smart_str_appendc(buf, '>');

	return SUCCESS;
}

static zend_result markup_render_fragment(smart_str *buf, zend_object *obj)
{
	zval rv;
	zval *children = zend_read_property(markup_ce_Fragment, obj, ZEND_STRL("children"), true, &rv);
	ZVAL_DEREF(children);
	if (Z_TYPE_P(children) != IS_ARRAY) {
		zend_throw_error(NULL, "Markup\\Fragment has not been initialized");
		return FAILURE;
	}
	return markup_append_child(buf, children, 0);
}

/* Bound the toHtml() resolution chain: a component may return another component,
 * but a class whose toHtml() returns itself (or a cycle) must not spin forever. */
#define MARKUP_TOHTML_MAX_DEPTH 64

/* Resolve an Html to one of the internal node classes (Element, Fragment,
 * Raw) by calling toHtml() until it lands on one. On SUCCESS *node holds an
 * owned reference; on FAILURE an exception is pending. */
static zend_result markup_resolve_html(zval *value, zval *node)
{
	zval current;
	ZVAL_COPY(&current, value);

	for (uint32_t depth = 0; depth < MARKUP_TOHTML_MAX_DEPTH; depth++) {
		zend_class_entry *ce = Z_OBJCE(current);
		if (ce == markup_ce_Element || ce == markup_ce_Fragment || ce == markup_ce_Raw) {
			ZVAL_COPY_VALUE(node, &current);
			return SUCCESS;
		}

		zval next;
		ZVAL_UNDEF(&next);
		zend_call_method_with_0_params(Z_OBJ(current), ce, NULL, "tohtml", &next);
		zval_ptr_dtor(&current);
		if (EG(exception)) {
			zval_ptr_dtor(&next);
			return FAILURE;
		}
		/* The declared return type already guarantees this for ordinary
		 * methods; guard anyway (e.g. a __call-backed implementation). */
		if (Z_TYPE(next) != IS_OBJECT || !instanceof_function(Z_OBJCE(next), markup_ce_Html)) {
			zend_throw_error(NULL,
				"%s::toHtml() did not return a Markup\\Html", ZSTR_VAL(ce->name));
			zval_ptr_dtor(&next);
			return FAILURE;
		}
		ZVAL_COPY_VALUE(&current, &next);
	}

	zend_throw_error(NULL,
		"Maximum toHtml() resolution depth of %d exceeded (%s::toHtml() never produced a Markup\\Element, Markup\\Fragment, or Markup\\Raw)",
		MARKUP_TOHTML_MAX_DEPTH, ZSTR_VAL(Z_OBJCE(current)->name));
	zval_ptr_dtor(&current);
	return FAILURE;
}

/* Serialize one of the internal node classes to its final HTML string.
 * Returns a new string, or NULL with an exception pending. */
static zend_string *markup_render_node(zval *node)
{
	zend_class_entry *ce = Z_OBJCE_P(node);

	if (ce == markup_ce_Raw) {
		zval rv;
		zval *html = zend_read_property(markup_ce_Raw, Z_OBJ_P(node), ZEND_STRL("html"), true, &rv);
		ZVAL_DEREF(html);
		if (Z_TYPE_P(html) != IS_STRING) {
			zend_throw_error(NULL, "Markup\\Raw has not been initialized");
			return NULL;
		}
		return zend_string_copy(Z_STR_P(html));
	}

	smart_str buf = {0};
	zend_result rendered = (ce == markup_ce_Element)
		? markup_render_element(&buf, Z_OBJ_P(node))
		: markup_render_fragment(&buf, Z_OBJ_P(node));
	if (rendered == FAILURE) {
		smart_str_free(&buf);
		return NULL;
	}
	return smart_str_extract(&buf);
}

/* Render any Html - a node class or a userland implementation - to its
 * final HTML string: resolve through toHtml(), then serialize natively.
 * Returns a new string, or NULL with an exception pending. */
static zend_string *markup_render_html(zval *value)
{
	zval node;
	if (markup_resolve_html(value, &node) == FAILURE) {
		return NULL;
	}
	zend_string *result = markup_render_node(&node);
	zval_ptr_dtor(&node);
	return result;
}

/* The default __toString() injected into userland Html classes at
 * class-link time (see markup_implement_html): render via toHtml(). */
static ZEND_NAMED_FUNCTION(markup_html_default_tostring)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_string *s = markup_render_html(ZEND_THIS);
	if (s == NULL) {
		RETURN_THROWS();
	}
	RETURN_STR(s);
}

/* interface_gets_implemented handler for Markup\Html: give every userland
 * implementing class a default __toString() unless it declares (or inherits)
 * one of its own, so `echo $component` / `(string) $component` always work
 * while toHtml() remains the only method a class must write.
 *
 * This runs during class linking, after the interface's methods were copied
 * into the class but before zend_verify_abstract_class(), so replacing the
 * inherited abstract __toString with a concrete function both satisfies
 * Stringable and keeps the class instantiable. The injected function follows
 * the engine's own precedent for enum cases()/from()/tryFrom(): an
 * arena-allocated internal function inside a userland class
 * (ZEND_ACC_ARENA_ALLOCATED is understood by class destruction, opcache
 * persistence, and the inheritance cache). */
static int markup_implement_html(zend_class_entry *iface, zend_class_entry *ce)
{
	if (ce->type != ZEND_USER_CLASS) {
		/* An internal class must ship its own __toString (Element, Fragment,
		 * and Raw do): injecting a request-arena-allocated default into a
		 * permanent class would dangle across requests. */
		if (ce->__tostring == NULL) {
			zend_error_noreturn(E_CORE_ERROR,
				"Internal class %s must declare __toString() to implement Markup\\Html",
				ZSTR_VAL(ce->name));
		}
		return SUCCESS;
	}

	/* A __toString declared by the class, inherited from a parent class, or
	 * flattened in from a trait wins; the default only fills the gap. When
	 * the entry exists here but is abstract, it is the unimplemented
	 * requirement inherited from Stringable through this interface. */
	zval *zv = zend_hash_find(&ce->function_table, markup_str_tostring_lc);
	if (zv != NULL && !(((zend_function *) Z_PTR_P(zv))->common.fn_flags & ZEND_ACC_ABSTRACT)) {
		return SUCCESS;
	}

	zend_internal_function *zif = zend_arena_calloc(&CG(arena), sizeof(zend_internal_function), 1);
	zif->type = ZEND_INTERNAL_FUNCTION;
	zif->module = EG(current_module);
	zif->scope = ce;
	zif->function_name = markup_str_tostring;
	zif->handler = markup_html_default_tostring;
	zif->doc_comment = NULL;
	zif->fn_flags = ZEND_ACC_PUBLIC | ZEND_ACC_HAS_RETURN_TYPE | ZEND_ACC_ARENA_ALLOCATED;
	zif->arg_info = markup_zarginfo_default_tostring + 1;
	zif->T = ZEND_OBSERVER_ENABLED;
	if (EG(active)) { /* linking at run-time (the normal case for user classes) */
		if (CG(compiler_options) & ZEND_COMPILE_PRELOAD) {
			zif->fn_flags |= ZEND_ACC_PRELOADED;
		}
		ZEND_MAP_PTR_INIT(zif->run_time_cache,
			zend_arena_calloc(&CG(arena), 1, zend_internal_run_time_cache_reserved_size()));
	} else {
#ifdef ZTS
		ZEND_MAP_PTR_NEW_STATIC(zif->run_time_cache);
#else
		ZEND_MAP_PTR_INIT(zif->run_time_cache, NULL);
#endif
	}

	if (zv != NULL) {
		/* Swap the abstract entry in place. The replaced pointer is the
		 * interface's own method, owned by the interface - nothing to free,
		 * and bypassing the hash dtor is exactly why this is not hash_update. */
		Z_PTR_P(zv) = (zend_function *) zif;
	} else {
		/* The hook can also fire before method inheritance (a class
		 * implementing an interface that extends Html reaches here through
		 * zend_do_inherit_interfaces); the later abstract-method copy then
		 * finds this concrete implementation and accepts it. */
		zend_hash_add_new_ptr(&ce->function_table, markup_str_tostring_lc, zif);
	}
	ce->__tostring = (zend_function *) zif;

	return SUCCESS;
}

/* Assigns a string to a (readonly) declared property during construction. */
static void markup_init_string_property(zend_class_entry *ce, zend_object *obj, const char *name, size_t name_len, zend_string *value)
{
	zval tmp;
	ZVAL_STR_COPY(&tmp, value);
	zend_update_property(ce, obj, name, name_len, &tmp);
	zval_ptr_dtor(&tmp);
}

static void markup_init_array_property(zend_class_entry *ce, zend_object *obj, const char *name, size_t name_len, zval *value /* may be NULL */)
{
	zval tmp;
	if (value != NULL) {
		ZVAL_COPY(&tmp, value);
	} else {
		ZVAL_EMPTY_ARRAY(&tmp);
	}
	zend_update_property(ce, obj, name, name_len, &tmp);
	zval_ptr_dtor(&tmp);
}

PHP_METHOD(Markup_Element, __construct)
{
	zend_string *tag;
	zval *attributes = NULL, *children = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(tag)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(attributes)
		Z_PARAM_ARRAY(children)
	ZEND_PARSE_PARAMETERS_END();

	zend_object *obj = Z_OBJ_P(ZEND_THIS);
	markup_init_string_property(markup_ce_Element, obj, ZEND_STRL("tag"), tag);
	markup_init_array_property(markup_ce_Element, obj, ZEND_STRL("attributes"), attributes);
	markup_init_array_property(markup_ce_Element, obj, ZEND_STRL("children"), children);
}

/* The node classes are the base cases of toHtml() resolution: each returns
 * itself. */
PHP_METHOD(Markup_Element, toHtml)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_COPY(ZEND_THIS);
}

PHP_METHOD(Markup_Fragment, toHtml)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_COPY(ZEND_THIS);
}

PHP_METHOD(Markup_Raw, toHtml)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_COPY(ZEND_THIS);
}

/* Shared __toString() body for the three node classes: serialize through
 * markup_render_node, the same function child-position rendering uses, so
 * `(string) $el` and rendering $el as a child can never diverge. */
static void markup_node_tostring(INTERNAL_FUNCTION_PARAMETERS)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_string *s = markup_render_node(ZEND_THIS);
	if (s == NULL) {
		RETURN_THROWS();
	}
	RETURN_STR(s);
}

PHP_METHOD(Markup_Element, __toString)
{
	markup_node_tostring(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Markup_Fragment, __construct)
{
	zval *children = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(children)
	ZEND_PARSE_PARAMETERS_END();

	markup_init_array_property(markup_ce_Fragment, Z_OBJ_P(ZEND_THIS), ZEND_STRL("children"), children);
}

PHP_METHOD(Markup_Fragment, __toString)
{
	markup_node_tostring(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Markup_Raw, __construct)
{
	zend_string *html;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(html)
	ZEND_PARSE_PARAMETERS_END();

	markup_init_string_property(markup_ce_Raw, Z_OBJ_P(ZEND_THIS), ZEND_STRL("html"), html);
}

PHP_METHOD(Markup_Raw, __toString)
{
	markup_node_tostring(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(Markup_LazyFragment, __construct)
{
	zval *thunk;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(thunk, zend_ce_closure)
	ZEND_PARSE_PARAMETERS_END();

	markup_lazy_fragment *lf = markup_lazy_fragment_from_obj(Z_OBJ_P(ZEND_THIS));
	ZVAL_COPY(&lf->thunk, thunk);
}

/* Resolve the deferred body once and memoize it: toHtml() is the seam the render
 * path already calls, so a component that never renders the slot never runs the
 * thunk. */
PHP_METHOD(Markup_LazyFragment, toHtml)
{
	ZEND_PARSE_PARAMETERS_NONE();

	markup_lazy_fragment *lf = markup_lazy_fragment_from_obj(Z_OBJ_P(ZEND_THIS));

	if (Z_TYPE(lf->resolved) == IS_UNDEF) {
		if (Z_TYPE(lf->thunk) != IS_OBJECT) {
			zend_throw_error(NULL, "Markup\\LazyFragment has not been initialized");
			RETURN_THROWS();
		}

		zend_fcall_info fci;
		zend_fcall_info_cache fcc;
		char *error = NULL;
		if (zend_fcall_info_init(&lf->thunk, 0, &fci, &fcc, NULL, &error) == FAILURE) {
			zend_throw_error(NULL, "Markup\\LazyFragment thunk is not callable%s%s",
				error ? ": " : "", error ? error : "");
			if (error) {
				efree(error);
			}
			RETURN_THROWS();
		}
		if (error) {
			efree(error);
		}

		zval result;
		ZVAL_UNDEF(&result);
		fci.retval = &result;
		zend_call_function(&fci, &fcc);

		if (EG(exception)) {
			zval_ptr_dtor(&result);
			RETURN_THROWS();
		}
		if (Z_TYPE(result) != IS_OBJECT
				|| !instanceof_function(Z_OBJCE(result), markup_ce_Html)) {
			zend_throw_error(NULL, "Markup\\LazyFragment thunk must return a Markup\\Html");
			zval_ptr_dtor(&result);
			RETURN_THROWS();
		}

		/* Move ownership into the memo. */
		ZVAL_COPY_VALUE(&lf->resolved, &result);
	}

	RETURN_COPY(&lf->resolved);
}

PHP_METHOD(Markup_LazyFragment, __toString)
{
	/* Identical to the default __toString() injected into userland Html
	 * classes: resolve through toHtml() (running/memoizing the thunk), then
	 * serialize. Declared explicitly because the injection skips internal
	 * classes (see markup_implement_html). */
	markup_html_default_tostring(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/* Build a Markup\Raw wrapping the given (already-final) HTML string. */
static void markup_return_raw(zval *return_value, zend_string *html)
{
	object_init_ex(return_value, markup_ce_Raw);
	markup_init_string_property(markup_ce_Raw, Z_OBJ_P(return_value), ZEND_STRL("html"), html);
}

PHP_FUNCTION(Markup_raw)
{
	zend_string *html;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(html)
	ZEND_PARSE_PARAMETERS_END();

	markup_return_raw(return_value, html);
}

PHP_FUNCTION(Markup_escape)
{
	zend_string *text;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(text)
	ZEND_PARSE_PARAMETERS_END();

	zend_string *escaped = markup_escape_string(ZSTR_VAL(text), ZSTR_LEN(text));
	markup_return_raw(return_value, escaped);
	zend_string_release(escaped);
}

/* Merge explicit props with body content routed via a #[Markup\Slot] parameter
 * attribute, producing the named-argument array for a component dispatch.
 * Returns a new array (caller releases) or NULL with a pending exception. */
static HashTable *markup_route_component_args(
	zend_function *fn, HashTable *props, zval *slot)
{
	HashTable *args = zend_new_array(props ? zend_hash_num_elements(props) : 0);
	zend_string *slot_param = NULL;       /* param name marked #[Markup\Slot] */

	/* 1. explicit props become named arguments */
	if (props) {
		zend_string *key;
		zval *val;
		ZEND_HASH_FOREACH_STR_KEY_VAL(props, key, val) {
			if (key == NULL) {
				zend_throw_error(NULL, "Component props must use string keys");
				goto fail;
			}
			Z_TRY_ADDREF_P(val);
			zend_hash_update(args, key, val);
		} ZEND_HASH_FOREACH_END();
	}

	/* 2. discover the slot parameter from a #[Markup\Slot] attribute */
	if (fn && fn->common.attributes) {
		for (uint32_t i = 0; i < fn->common.num_args; i++) {
			zend_attribute *attr = zend_get_parameter_attribute_str(
				fn->common.attributes, ZEND_STRL("markup\\slot"), i);
			if (attr == NULL) {
				continue;
			}
			if (slot_param != NULL) {
				zend_throw_error(NULL, "Component has more than one #[Markup\\Slot] parameter");
				goto fail;
			}
			slot_param = fn->common.arg_info[i].name;
		}

		/* A slot is a single value, so it cannot fill a variadic parameter
		 * (which num_args excludes above); reject rather than silently ignore. */
		if (fn->common.fn_flags & ZEND_ACC_VARIADIC
				&& zend_get_parameter_attribute_str(
					fn->common.attributes, ZEND_STRL("markup\\slot"), fn->common.num_args) != NULL) {
			zend_throw_error(NULL, "#[Markup\\Slot] cannot be applied to a variadic parameter");
			goto fail;
		}
	}

	/* 3. route the body slot */
	if (slot != NULL) {
		if (slot_param == NULL) {
			zend_throw_error(NULL, "Component received body content but has no #[Markup\\Slot] parameter");
			goto fail;
		}
		if (zend_hash_exists(args, slot_param)) {
			zend_throw_error(NULL, "Parameter $%s is filled by both an attribute and body content", ZSTR_VAL(slot_param));
			goto fail;
		}
		Z_ADDREF_P(slot);
		zend_hash_add_new(args, slot_param, slot);
	}

	return args;

fail:
	zend_array_release(args);
	return NULL;
}

/* One entry in a hook list: the callable plus an optional component scope.
 * A NULL scope means the hook runs for every component; a non-NULL scope
 * limits it to the one component whose resolved name matches (the same name
 * the hook receives as its string argument). Scoped hooks let the dispatch
 * skip non-matching handlers in C, before any userland call. */
typedef struct {
	zval callable;
	zend_string *component;
} markup_handler;

static void markup_handler_dtor(zval *zv)
{
	markup_handler *handler = Z_PTR_P(zv);
	zval_ptr_dtor(&handler->callable);
	if (handler->component != NULL) {
		zend_string_release(handler->component);
	}
	efree(handler);
}

/* Component names are case-insensitive everywhere PHP resolves them (classes,
 * functions, methods), so scope matching is too. */
static zend_always_inline bool markup_handler_matches(const markup_handler *handler, zend_string *name)
{
	return handler->component == NULL || zend_string_equals_ci(handler->component, name);
}

/* Resolved component names never carry a leading backslash, but "\App\Card"
 * is how userland habitually writes an FQCN; accept both. */
static zend_string *markup_normalize_component_scope(zend_string *component)
{
	if (ZSTR_LEN(component) > 0 && ZSTR_VAL(component)[0] == '\\') {
		return zend_string_init(ZSTR_VAL(component) + 1, ZSTR_LEN(component) - 1, 0);
	}
	return zend_string_copy(component);
}

/* Append a callable to one of the request-scoped handler lists (allocating it on
 * first use). Adds its own reference to the handler. Throws a TypeError and
 * returns false when the value is not callable (like spl_autoload_register). */
static bool markup_register_handler(HashTable **list, zval *callable, zend_string *component, uint32_t arg_num)
{
	if (!zend_is_callable(callable, 0, NULL)) {
		zend_argument_type_error(arg_num, "must be a valid callback");
		return false;
	}
	if (*list == NULL) {
		ALLOC_HASHTABLE(*list);
		zend_hash_init(*list, 4, NULL, markup_handler_dtor, 0);
	}
	markup_handler *handler = emalloc(sizeof(markup_handler));
	ZVAL_COPY(&handler->callable, callable);
	handler->component = component != NULL ? markup_normalize_component_scope(component) : NULL;
	zend_hash_next_index_insert_ptr(*list, handler);
	return true;
}

/* Remove the first handler identical to the given callable *and* registered
 * with the same scope (unscoped only matches unscoped). Returns whether one
 * matched. Order of the remaining handlers is preserved. */
static bool markup_unregister_handler(HashTable *list, zval *callable, zend_string *component)
{
	if (list == NULL) {
		return false;
	}
	zend_string *scope = component != NULL ? markup_normalize_component_scope(component) : NULL;
	bool removed = false;
	zend_ulong idx;
	markup_handler *handler;
	ZEND_HASH_FOREACH_NUM_KEY_PTR(list, idx, handler) {
		if (!zend_is_identical(&handler->callable, callable)) {
			continue;
		}
		if ((handler->component == NULL) != (scope == NULL)
				|| (scope != NULL && !zend_string_equals_ci(handler->component, scope))) {
			continue;
		}
		removed = zend_hash_index_del(list, idx) == SUCCESS;
		break;
	} ZEND_HASH_FOREACH_END();
	if (scope != NULL) {
		zend_string_release(scope);
	}
	return removed;
}

/* A fresh, always-refcounted array copy of the routed component arguments (or an
 * empty array when there are none), safe to hand to a userland factory.
 * Duplicating avoids force-refcounting a possibly-immutable borrowed array (an
 * empty `[]` literal is the shared, read-only zend_empty_array) and stops a
 * handler from mutating the engine's internal argument array. */
static zend_array *markup_args_for_handler(HashTable *args)
{
	return args != NULL ? zend_array_dup(args) : zend_new_array(0);
}

/* Snapshot the handlers in `chain` whose scope matches `name`, or NULL when
 * none match. No user code runs during the scan, so iterating the live table
 * is safe; the snapshot exists because the handlers themselves may
 * (un)register hooks, which would otherwise rehash the table under a live
 * iterator (a registration made during dispatch takes effect from the next
 * dispatch). A fully non-matching dispatch allocates nothing. */
static HashTable *markup_matching_handlers(HashTable *chain, zend_string *name)
{
	HashTable *snapshot = NULL;
	markup_handler *handler;
	ZEND_HASH_FOREACH_PTR(chain, handler) {
		if (!markup_handler_matches(handler, name)) {
			continue;
		}
		if (snapshot == NULL) {
			ALLOC_HASHTABLE(snapshot);
			zend_hash_init(snapshot, 4, NULL, ZVAL_PTR_DTOR, 0);
		}
		Z_TRY_ADDREF(handler->callable);
		zend_hash_next_index_insert_new(snapshot, &handler->callable);
	} ZEND_HASH_FOREACH_END();
	return snapshot;
}

/* Run a handler chain (component factories). Each in-scope handler
 * is called as `handler(name, args)`; the first to return a non-null value
 * takes over the dispatch. Returns 1 with *result set (owned) when a handler
 * took it, 0 when every handler deferred (or none are in scope), or -1 when a
 * handler threw. The userland args copy is only made once a handler is
 * actually about to run. */
static int markup_run_handler_chain(HashTable *chain, zend_string *name, HashTable *args, zval *result)
{
	if (chain == NULL) {
		return 0;
	}
	HashTable *snapshot = markup_matching_handlers(chain, name);
	if (snapshot == NULL) {
		return 0;
	}
	zval name_zv, args_zv;
	ZVAL_STR_COPY(&name_zv, name);
	ZVAL_ARR(&args_zv, markup_args_for_handler(args));
	int outcome = 0;
	zval *handler;
	ZEND_HASH_FOREACH_VAL(snapshot, handler) {
		zval params[2], ret;
		ZVAL_COPY(&params[0], &name_zv);
		ZVAL_COPY(&params[1], &args_zv);
		ZVAL_UNDEF(&ret);
		zend_result called = call_user_function(NULL, NULL, handler, &ret, 2, params);
		zval_ptr_dtor(&params[0]);
		zval_ptr_dtor(&params[1]);
		if (called != SUCCESS || EG(exception)) {
			zval_ptr_dtor(&ret);
			outcome = -1;
			break;
		}
		if (Z_TYPE(ret) != IS_NULL) {
			ZVAL_COPY_VALUE(result, &ret);
			outcome = 1;
			break;
		}
		zval_ptr_dtor(&ret);
	} ZEND_HASH_FOREACH_END();
	zval_ptr_dtor(&name_zv);
	zval_ptr_dtor(&args_zv);
	zend_hash_release(snapshot);
	return outcome;
}

PHP_FUNCTION(Markup_register_component_factory)
{
	zval *factory;
	zend_string *component = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(factory)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(component)
	ZEND_PARSE_PARAMETERS_END();

	if (!markup_register_handler(&MARKUP_G(component_factories), factory, component, 1)) {
		RETURN_THROWS();
	}
}

PHP_FUNCTION(Markup_unregister_component_factory)
{
	zval *factory;
	zend_string *component = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(factory)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(component)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_is_callable(factory, 0, NULL)) {
		zend_argument_type_error(1, "must be a valid callback");
		RETURN_THROWS();
	}

	RETURN_BOOL(markup_unregister_handler(MARKUP_G(component_factories), factory, component));
}

PHP_FUNCTION(Markup_register_component_decorator)
{
	zval *decorator;
	zend_string *component = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(decorator)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(component)
	ZEND_PARSE_PARAMETERS_END();

	if (!markup_register_handler(&MARKUP_G(component_decorators), decorator, component, 1)) {
		RETURN_THROWS();
	}
}

PHP_FUNCTION(Markup_unregister_component_decorator)
{
	zval *decorator;
	zend_string *component = NULL;
	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(decorator)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR_OR_NULL(component)
	ZEND_PARSE_PARAMETERS_END();

	if (!zend_is_callable(decorator, 0, NULL)) {
		zend_argument_type_error(1, "must be a valid callback");
		RETURN_THROWS();
	}

	RETURN_BOOL(markup_unregister_handler(MARKUP_G(component_decorators), decorator, component));
}

/* Run the produced Markup\Html through the decorator chain: each decorator is
 * called as `decorator(Markup\Html $rendered, string $component)` and returns a
 * possibly-wrapped Markup\Html, composed in registration order. Updates *html in
 * place; on failure returns false with an exception pending and *html released. */
static bool markup_apply_decorators(zend_string *component, zval *html)
{
	if (MARKUP_G(component_decorators) == NULL) {
		return true;
	}
	HashTable *decorators = markup_matching_handlers(MARKUP_G(component_decorators), component);
	if (decorators == NULL) {
		return true;
	}
	zval name_zv;
	ZVAL_STR_COPY(&name_zv, component);

	zval *decorator;
	ZEND_HASH_FOREACH_VAL(decorators, decorator) {
		zval params[2], decorated;
		ZVAL_COPY(&params[0], html);
		ZVAL_COPY(&params[1], &name_zv);
		ZVAL_UNDEF(&decorated);
		zend_result called = call_user_function(NULL, NULL, decorator, &decorated, 2, params);
		zval_ptr_dtor(&params[0]);
		zval_ptr_dtor(&params[1]);
		if (called != SUCCESS || EG(exception)) {
			zval_ptr_dtor(&decorated);
			goto fail;
		}
		if (Z_TYPE(decorated) != IS_OBJECT || !instanceof_function(Z_OBJCE(decorated), markup_ce_Html)) {
			zend_throw_error(NULL,
				"Component decorator for <%s> did not return a Markup\\Html",
				ZSTR_VAL(component));
			zval_ptr_dtor(&decorated);
			goto fail;
		}
		zval_ptr_dtor(html);
		ZVAL_COPY_VALUE(html, &decorated);
	} ZEND_HASH_FOREACH_END();

	zend_array_release(decorators);
	zval_ptr_dtor(&name_zv);
	return true;

fail:
	zend_array_release(decorators);
	zval_ptr_dtor(&name_zv);
	zval_ptr_dtor(html);
	return false;
}

/* Resolve a "Class::method" component to a public static user-defined method.
 * On FAILURE an exception is pending. */
static zend_result markup_resolve_static_method(
	zend_string *component, const char *sep, zend_class_entry **ce_out, zend_function **fn_out)
{
	size_t class_len = sep - ZSTR_VAL(component);
	zend_string *class_name = zend_string_init(ZSTR_VAL(component), class_len, 0);
	zend_string *method_name = zend_string_init(sep + 2, ZSTR_LEN(component) - class_len - 2, 0);
	zend_class_entry *ce;
	zend_function *fn;
	zend_result result = FAILURE;

	ce = zend_lookup_class(class_name);
	if (ce == NULL) {
		zend_throw_error(NULL, "Component class \"%s\" not found", ZSTR_VAL(class_name));
		goto out;
	}

	zend_string *lc_method = zend_string_tolower(method_name);
	fn = zend_hash_find_ptr(&ce->function_table, lc_method);
	zend_string_release(lc_method);

	if (fn == NULL) {
		zend_throw_error(NULL, "Component method %s::%s() not found",
			ZSTR_VAL(class_name), ZSTR_VAL(method_name));
		goto out;
	}

	/* The engine calls the method from no particular scope with no $this, so
	 * only a public, non-abstract *static* method is a valid component target
	 * (a raw function-table hit bypasses the VM's own checks). Internal
	 * methods are rejected like internal functions. */
	if (!(fn->common.fn_flags & ZEND_ACC_STATIC)
			|| !(fn->common.fn_flags & ZEND_ACC_PUBLIC)
			|| (fn->common.fn_flags & ZEND_ACC_ABSTRACT)
			|| fn->type == ZEND_INTERNAL_FUNCTION) {
		zend_throw_error(NULL,
			"Component method %s::%s() must be a public static user-defined method",
			ZSTR_VAL(class_name), ZSTR_VAL(method_name));
		goto out;
	}

	*ce_out = ce;
	*fn_out = fn;
	result = SUCCESS;

out:
	zend_string_release(class_name);
	zend_string_release(method_name);
	return result;
}

/* Dispatch a static-method component: call it and require a Markup\Html
 * back. Same contract as markup_dispatch_new(): the result goes into *result on
 * SUCCESS, an exception is pending on FAILURE, and `args` is never released. */
static zend_result markup_dispatch_call(
	zend_function *fn, zend_class_entry *called_scope, zend_string *component,
	HashTable *args, zval *result)
{
	zval ret;
	ZVAL_UNDEF(&ret);
	zend_call_known_function(fn, NULL, called_scope, &ret, 0, NULL, args);
	if (EG(exception)) {
		zval_ptr_dtor(&ret);
		return FAILURE;
	}
	if (Z_TYPE(ret) != IS_OBJECT || !instanceof_function(Z_OBJCE(ret), markup_ce_Html)) {
		zend_throw_error(NULL,
			"Component \"%s\" did not return a Markup\\Html", ZSTR_VAL(component));
		zval_ptr_dtor(&ret);
		return FAILURE;
	}
	ZVAL_COPY_VALUE(result, &ret);
	return SUCCESS;
}

/* Dispatch a class component: a registered factory may construct it (e.g.
 * through a DI container) before the engine's own `new`. On SUCCESS the
 * produced object is in *result; on FAILURE an exception is pending. Never
 * releases `args` - the caller owns it. */
static zend_result markup_dispatch_new(
	zend_class_entry *ce, zend_function *ctor, HashTable *args, zval *result)
{
	if (MARKUP_G(component_factories) != NULL) {
		zval obj;
		int handled = markup_run_handler_chain(MARKUP_G(component_factories), ce->name, args, &obj);
		if (handled < 0) {
			return FAILURE;
		}
		if (handled) {
			if (Z_TYPE(obj) != IS_OBJECT || !instanceof_function(Z_OBJCE(obj), ce)) {
				zend_throw_error(NULL,
					"Component factory for <%s> did not return an instance of it or null",
					ZSTR_VAL(ce->name));
				zval_ptr_dtor(&obj);
				return FAILURE;
			}
			ZVAL_COPY_VALUE(result, &obj);
			return SUCCESS;
		}
	}

	/* Mirror what `new` enforces: the engine constructs from no particular
	 * scope, so the constructor must be public; and a class without a
	 * constructor cannot receive props/slots (dropping them silently would
	 * hide the mistake). */
	if (ctor != NULL && !(ctor->common.fn_flags & ZEND_ACC_PUBLIC)) {
		zend_throw_error(NULL, "Component class \"%s\" has a non-public constructor",
			ZSTR_VAL(ce->name));
		return FAILURE;
	}
	if (ctor == NULL && args != NULL && zend_hash_num_elements(args) > 0) {
		zend_throw_error(NULL,
			"Component class \"%s\" has no constructor, so it cannot accept attributes or slots",
			ZSTR_VAL(ce->name));
		return FAILURE;
	}

	zval obj;
	if (object_init_ex(&obj, ce) != SUCCESS) {
		return FAILURE;
	}
	if (ctor != NULL) {
		zval ctor_ret;
		ZVAL_UNDEF(&ctor_ret);
		zend_call_known_function(ctor, Z_OBJ(obj), ce, &ctor_ret, 0, NULL, args);
		zval_ptr_dtor(&ctor_ret);
		if (EG(exception)) {
			zval_ptr_dtor(&obj);
			return FAILURE;
		}
	}
	ZVAL_COPY_VALUE(result, &obj);
	return SUCCESS;
}

/* The body of Markup\render_component(), shared with Markup\render_dynamic()'s
 * component path. Same contract as a PHP_FUNCTION body: the produced
 * Markup\Html goes into *return_value, or an exception is left pending. */
static void markup_render_component_impl(
	zend_string *component, HashTable *props, zend_object *slot_obj,
	zval *return_value)
{
	/* Resolve the component to a dispatch target. This is the second
	 * stage of a two-stage resolution: the compiler already resolved the *name*
	 * against use/namespace (see zend_ast_create_markup_component in
	 * Zend/zend_markup.c); here that name resolves to a class implementing
	 * Markup\Html (instantiated) or, for "Class::method", a public static
	 * method (called). Class resolution is the only name machinery involved -
	 * plain functions are not components (see the RFC's Future Scope). */
	zend_function *fn;
	zend_class_entry *ce;
	bool is_new;

	const char *end = ZSTR_VAL(component) + ZSTR_LEN(component);
	const char *sep = zend_memnstr(ZSTR_VAL(component), "::", 2, end);

	if (sep != NULL) {
		/* "Class::method" - an explicit static-method component. */
		if (markup_resolve_static_method(component, sep, &ce, &fn) == FAILURE) {
			RETURN_THROWS();
		}
		is_new = false;
	} else {
		ce = zend_lookup_class(component);
		if (ce == NULL || !instanceof_function(ce, markup_ce_Html)) {
			zend_throw_error(NULL,
				"\"%s\" is not a component: no such class implementing Markup\\Html",
				ZSTR_VAL(component));
			RETURN_THROWS();
		}
		fn = ce->constructor;
		is_new = true;
	}

	/* Build the named-argument array (props + routed slots). */
	zval slot_zv;
	zval *slot_ptr = NULL;
	if (slot_obj != NULL) {
		ZVAL_OBJ(&slot_zv, slot_obj);
		slot_ptr = &slot_zv;
	}

	/* Fast path: with no slot content to route and no #[Markup\Slot] parameters to
	 * inspect, the named-argument array is exactly $props - pass it through
	 * directly (borrowed) instead of allocating and copying a new HashTable.
	 * Each path validates prop keys exactly once: here on the borrowed table
	 * (an integer key would silently become a positional argument in
	 * zend_call_known_function), or in markup_route_component_args as it copies. */
	HashTable *args;
	bool owns_args;
	if (slot_ptr == NULL
			&& (fn == NULL || fn->common.attributes == NULL)) {
		if (props != NULL) {
			zend_string *key;
			ZEND_HASH_FOREACH_STR_KEY(props, key) {
				if (key == NULL) {
					zend_throw_error(NULL, "Component props must use string keys");
					RETURN_THROWS();
				}
			} ZEND_HASH_FOREACH_END();
		}
		args = props; /* may be NULL when no attributes were given */
		owns_args = false;
	} else {
		args = markup_route_component_args(fn, props, slot_ptr);
		if (args == NULL) {
			RETURN_THROWS();
		}
		owns_args = true;
	}

	/* Dispatch: either path produces the component's Markup\Html into `result`
	 * and never releases `args`, so the single release below covers every
	 * outcome. */
	zval result;
	ZVAL_UNDEF(&result);
	zend_result dispatched = is_new
		? markup_dispatch_new(ce, fn, args, &result)
		: markup_dispatch_call(fn, ce, component, args, &result);

	if (owns_args) {
		zend_array_release(args);
	}
	if (dispatched == FAILURE) {
		RETURN_THROWS();
	}

	/* Decorators wrap the produced Html uniformly, whatever its kind. A
	 * class component's name is the resolved ce->name (which may differ in
	 * case/aliasing from the tag); a static method's is the "Class::method"
	 * string, whose class part the compiler already resolved. */
	if (!markup_apply_decorators(is_new ? ce->name : component, &result)) {
		RETURN_THROWS();
	}

	RETURN_COPY_VALUE(&result);
}

PHP_FUNCTION(Markup_render_component)
{
	zend_string *component;
	HashTable *props = NULL;
	zend_object *slot_obj = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(component)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT(props)
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(slot_obj, markup_ce_Html)
	ZEND_PARSE_PARAMETERS_END();

	markup_render_component_impl(component, props, slot_obj, return_value);
}

/* The runtime target of a dynamic tag `<$tag …>…</$tag>`. The value
 * decides at runtime exactly what a static tag name decides at compile time,
 * by the same classification rule (zend_markup_name_is_component): an
 * uppercase first character, a "\", or a "::" dispatches as a component -
 * through the full render_component machinery, hooks and decorators included -
 * and anything else constructs a literal \Markup\Element. Exposed as a public
 * function like render_component, so the rule is directly testable. */
PHP_FUNCTION(Markup_render_dynamic)
{
	zend_string *tag;
	zval *attributes = NULL;
	zval *children = NULL;

	/* The array parameters are taken as zvals, not raw HashTables: they are
	 * forwarded to constructor calls below, and a fresh ZVAL_ARR of a borrowed
	 * table would mark an immutable array literal (e.g. a compiled []) as
	 * refcounted, making the callee addref read-only memory. The caller's
	 * zvals carry the correct type flags. */
	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(tag)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY(attributes)
		Z_PARAM_ARRAY(children)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(tag) == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	bool have_children = children != NULL && zend_hash_num_elements(Z_ARRVAL_P(children)) > 0;

	if (zend_markup_name_is_component(tag)) {
		/* Body children become the slot Fragment, exactly as the compiler builds
		 * one for a static component tag. A dynamic component name is already
		 * fully qualified (e.g. Foo::class, or "Class::method" for a static
		 * method) and resolves directly. */
		zval slot;
		zend_object *slot_obj = NULL;
		ZVAL_UNDEF(&slot);
		if (have_children) {
			if (object_init_ex(&slot, markup_ce_Fragment) != SUCCESS) {
				RETURN_THROWS();
			}
			slot_obj = Z_OBJ(slot);
			zend_call_known_function(markup_ce_Fragment->constructor, slot_obj,
				markup_ce_Fragment, NULL, 1, children, NULL);
			if (EG(exception)) {
				zval_ptr_dtor(&slot);
				RETURN_THROWS();
			}
		}
		markup_render_component_impl(tag,
			attributes != NULL ? Z_ARRVAL_P(attributes) : NULL, slot_obj,
			return_value);
		zval_ptr_dtor(&slot);
		return;
	}

	zval args[3];
	ZVAL_STR(&args[0], tag);
	if (attributes != NULL) {
		ZVAL_COPY_VALUE(&args[1], attributes);
	} else {
		ZVAL_EMPTY_ARRAY(&args[1]);
	}
	if (children != NULL) {
		ZVAL_COPY_VALUE(&args[2], children);
	} else {
		ZVAL_EMPTY_ARRAY(&args[2]);
	}

	if (object_init_ex(return_value, markup_ce_Element) != SUCCESS) {
		RETURN_THROWS();
	}
	zend_call_known_function(markup_ce_Element->constructor, Z_OBJ_P(return_value),
		markup_ce_Element, NULL, 3, args, NULL);
	if (EG(exception)) {
		zval_ptr_dtor(return_value);
		ZVAL_UNDEF(return_value);
		RETURN_THROWS();
	}
}

static PHP_GINIT_FUNCTION(markup)
{
	markup_globals->component_factories = NULL;
	markup_globals->component_decorators = NULL;
}

PHP_RSHUTDOWN_FUNCTION(markup)
{
	/* Component hooks are request-scoped; drop the ones registered this request. */
	if (MARKUP_G(component_factories) != NULL) {
		zend_hash_release(MARKUP_G(component_factories));
		MARKUP_G(component_factories) = NULL;
	}
	if (MARKUP_G(component_decorators) != NULL) {
		zend_hash_release(MARKUP_G(component_decorators));
		MARKUP_G(component_decorators) = NULL;
	}
	return SUCCESS;
}

PHP_MINIT_FUNCTION(markup)
{
	markup_ce_Html = register_class_Markup_Html(zend_ce_stringable);
	markup_ce_Html->interface_gets_implemented = markup_implement_html;
	markup_str_tostring = zend_string_init_interned(ZEND_STRL("__toString"), true);
	markup_str_tostring_lc = zend_string_init_interned(ZEND_STRL("__tostring"), true);
	for (size_t i = 0; i < sizeof(markup_zarginfo_default_tostring) / sizeof(zend_arg_info); i++) {
		zend_convert_internal_arg_info(&markup_zarginfo_default_tostring[i],
			&arginfo_class_Markup_Html___toString[i], i == 0, true);
	}

	markup_ce_Element = register_class_Markup_Element(markup_ce_Html);
	markup_ce_Fragment = register_class_Markup_Fragment(markup_ce_Html);
	markup_ce_Raw = register_class_Markup_Raw(markup_ce_Html);
	markup_ce_Slot = register_class_Markup_Slot();

	markup_ce_LazyFragment = register_class_Markup_LazyFragment(markup_ce_Html);
	markup_ce_LazyFragment->create_object = markup_lazy_fragment_create;
	memcpy(&markup_lazy_fragment_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	markup_lazy_fragment_handlers.offset = offsetof(markup_lazy_fragment, std);
	markup_lazy_fragment_handlers.free_obj = markup_lazy_fragment_free;
	markup_lazy_fragment_handlers.get_gc = markup_lazy_fragment_get_gc;
	markup_lazy_fragment_handlers.clone_obj = NULL;

	/* The parser lowers markup to these fully-qualified names (zend_markup.h);
	 * assert the runtime registers exactly them so the two can never drift. */
	ZEND_ASSERT(zend_string_equals_literal(markup_ce_Element->name, ZEND_MARKUP_ELEMENT_FQ));
	ZEND_ASSERT(zend_string_equals_literal(markup_ce_Fragment->name, ZEND_MARKUP_FRAGMENT_FQ));
	ZEND_ASSERT(zend_string_equals_literal(markup_ce_LazyFragment->name, ZEND_MARKUP_LAZY_FRAGMENT_FQ));

	return SUCCESS;
}

PHP_MINFO_FUNCTION(markup)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "markup support", "enabled");
	php_info_print_table_end();
}

zend_module_entry markup_module_entry = {
	STANDARD_MODULE_HEADER,
	"markup",					/* Extension name */
	ext_functions,			/* zend_function_entry */
	PHP_MINIT(markup),		/* PHP_MINIT - Module initialization */
	NULL,					/* PHP_MSHUTDOWN - Module shutdown */
	NULL,					/* PHP_RINIT - Request initialization */
	PHP_RSHUTDOWN(markup),	/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(markup),		/* PHP_MINFO - Module info */
	PHP_MARKUP_VERSION,		/* Version */
	PHP_MODULE_GLOBALS(markup),	/* Module globals */
	PHP_GINIT(markup),		/* PHP_GINIT - Global initialization */
	NULL,					/* PHP_GSHUTDOWN - Global shutdown */
	NULL,					/* Post deactivate */
	STANDARD_MODULE_PROPERTIES_EX,
};

#ifdef COMPILE_DL_MARKUP
ZEND_GET_MODULE(markup)
#endif
