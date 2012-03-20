/**********************************************************************

  debug.c -

  $Author$
  created at: 04/08/25 02:31:54 JST

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include "ruby/util.h"
#include "debug.h"
#include "eval_intern.h"
#include "vm_core.h"
#include "id.h"

/* for gdb */
const union {
    enum ruby_special_consts    special_consts;
    enum ruby_value_type        value_type;
    enum ruby_tag_type          tag_type;
    enum node_type              node_type;
    enum ruby_method_ids        method_ids;
    enum {
        RUBY_ENCODING_INLINE_MAX = ENCODING_INLINE_MAX,
        RUBY_ENCODING_SHIFT = ENCODING_SHIFT,
        RUBY_ENC_CODERANGE_MASK    = ENC_CODERANGE_MASK,
        RUBY_ENC_CODERANGE_UNKNOWN = ENC_CODERANGE_UNKNOWN,
        RUBY_ENC_CODERANGE_7BIT    = ENC_CODERANGE_7BIT,
        RUBY_ENC_CODERANGE_VALID   = ENC_CODERANGE_VALID,
        RUBY_ENC_CODERANGE_BROKEN  = ENC_CODERANGE_BROKEN,
        RUBY_FL_RESERVED1   = FL_RESERVED1,
        RUBY_FL_RESERVED2   = FL_RESERVED2,
        RUBY_FL_FINALIZE    = FL_FINALIZE,
        RUBY_FL_TAINT       = FL_TAINT,
        RUBY_FL_UNTRUSTED   = FL_UNTRUSTED,
        RUBY_FL_EXIVAR      = FL_EXIVAR,
        RUBY_FL_FREEZE      = FL_FREEZE,
        RUBY_FL_SINGLETON   = FL_SINGLETON,
        RUBY_FL_USER0       = FL_USER0,
        RUBY_FL_USER1       = FL_USER1,
        RUBY_FL_USER2       = FL_USER2,
        RUBY_FL_USER3       = FL_USER3,
        RUBY_FL_USER4       = FL_USER4,
        RUBY_FL_USER5       = FL_USER5,
        RUBY_FL_USER6       = FL_USER6,
        RUBY_FL_USER7       = FL_USER7,
        RUBY_FL_USER8       = FL_USER8,
        RUBY_FL_USER9       = FL_USER9,
        RUBY_FL_USER10      = FL_USER10,
        RUBY_FL_USER11      = FL_USER11,
        RUBY_FL_USER12      = FL_USER12,
        RUBY_FL_USER13      = FL_USER13,
        RUBY_FL_USER14      = FL_USER14,
        RUBY_FL_USER15      = FL_USER15,
        RUBY_FL_USER16      = FL_USER16,
        RUBY_FL_USER17      = FL_USER17,
        RUBY_FL_USER18      = FL_USER18,
        RUBY_FL_USHIFT      = FL_USHIFT,
        RUBY_NODE_TYPESHIFT = NODE_TYPESHIFT,
        RUBY_NODE_TYPEMASK  = NODE_TYPEMASK,
        RUBY_NODE_LSHIFT    = NODE_LSHIFT,
        RUBY_NODE_FL_NEWLINE   = NODE_FL_NEWLINE
    } various;
} ruby_dummy_gdb_enums;

const VALUE RUBY_FL_USER19    = FL_USER19;
const SIGNED_VALUE RUBY_NODE_LMASK = NODE_LMASK;
const VALUE RUBY_ENCODING_MASK  = ENCODING_MASK;

int
ruby_debug_print_indent(int level, int debug_level, int indent_level)
{
    if (level < debug_level) {
	fprintf(stderr, "%*s", indent_level, "");
	fflush(stderr);
	return TRUE;
    }
    return FALSE;
}

void
ruby_debug_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

VALUE
ruby_debug_print_value(int level, int debug_level, const char *header, VALUE obj)
{
    if (level < debug_level) {
	VALUE str;
	str = rb_inspect(obj);
	fprintf(stderr, "DBG> %s: %s\n", header,
		obj == (VALUE)(SIGNED_VALUE)-1 ? "" : StringValueCStr(str));
	fflush(stderr);
    }
    return obj;
}

void
ruby_debug_print_v(VALUE v)
{
    ruby_debug_print_value(0, 1, "", v);
}

ID
ruby_debug_print_id(int level, int debug_level, const char *header, ID id)
{
    if (level < debug_level) {
	fprintf(stderr, "DBG> %s: %s\n", header, rb_id2name(id));
	fflush(stderr);
    }
    return id;
}

NODE *
ruby_debug_print_node(int level, int debug_level, const char *header, const NODE *node)
{
    if (level < debug_level) {
	fprintf(stderr, "DBG> %s: %s (%u)\n", header,
		ruby_node_name(nd_type(node)), nd_line(node));
    }
    return (NODE *)node;
}

void
ruby_debug_breakpoint(void)
{
    /* */
}

static void
set_debug_option(const char *str, int len, void *arg)
{
#define SET_WHEN(name, var) do {	    \
	if (len == sizeof(name) - 1 &&	    \
	    strncmp(str, (name), len) == 0) { \
	    extern int var;	    \
	    var = 1;		    \
	    return;			    \
	}				    \
    } while (0)
    SET_WHEN("gc_stress", *ruby_initial_gc_stress_ptr);
    SET_WHEN("core", ruby_enable_coredump);
#if defined _WIN32 && defined _MSC_VER && _MSC_VER >= 1400
    SET_WHEN("rtc_error", ruby_w32_rtc_error);
#endif
    fprintf(stderr, "unexpected debug option: %.*s\n", len, str);
}

void
ruby_set_debug_option(const char *str)
{
    ruby_each_words(str, set_debug_option, 0);
}
