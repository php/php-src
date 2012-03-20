/*

    cparse.c -- Racc Runtime Core

    Copyright (c) 1999-2006 Minero Aoki

    This library is free software.
    You can distribute/modify this program under the same terms of ruby.

    $originalId: cparse.c,v 1.8 2006/07/06 11:39:46 aamine Exp $

*/

#include "ruby/ruby.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/* -----------------------------------------------------------------------
                        Important Constants
----------------------------------------------------------------------- */

#define RACC_VERSION "1.4.5"

#define DEFAULT_TOKEN -1
#define ERROR_TOKEN    1
#define FINAL_TOKEN    0

#define vDEFAULT_TOKEN  INT2FIX(DEFAULT_TOKEN)
#define vERROR_TOKEN    INT2FIX(ERROR_TOKEN)
#define vFINAL_TOKEN    INT2FIX(FINAL_TOKEN)

/* -----------------------------------------------------------------------
                          File Local Variables
----------------------------------------------------------------------- */

static VALUE RaccBug;
static VALUE CparseParams;

static ID id_yydebug;
static ID id_nexttoken;
static ID id_onerror;
static ID id_noreduce;
static ID id_errstatus;

static ID id_d_shift;
static ID id_d_reduce;
static ID id_d_accept;
static ID id_d_read_token;
static ID id_d_next_state;
static ID id_d_e_pop;

/* -----------------------------------------------------------------------
                              Utils
----------------------------------------------------------------------- */

/* For backward compatibility */
#ifndef ID2SYM
# define ID2SYM(i) ULONG2NUM(i)
#endif
#ifndef SYM2ID
#  define SYM2ID(v) ((ID)NUM2ULONG(v))
#endif
#ifndef SYMBOL_P
#  define SYMBOL_P(v) FIXNUM_P(v)
#endif
#ifndef LONG2NUM
#  define LONG2NUM(i) INT2NUM(i)
#endif

static ID value_to_id _((VALUE v));
static inline long num_to_long _((VALUE n));

static ID
value_to_id(VALUE v)
{
    if (! SYMBOL_P(v)) {
        rb_raise(rb_eTypeError, "not symbol");
    }
    return SYM2ID(v);
}

static inline long
num_to_long(VALUE n)
{
    return NUM2LONG(n);
}

#define AREF(s, idx) \
    ((0 <= idx && idx < RARRAY_LEN(s)) ? RARRAY_PTR(s)[idx] : Qnil)

/* -----------------------------------------------------------------------
                        Parser Stack Interfaces
----------------------------------------------------------------------- */

static VALUE get_stack_tail _((VALUE stack, long len));
static void cut_stack_tail _((VALUE stack, long len));

static VALUE
get_stack_tail(VALUE stack, long len)
{
    if (len < 0) return Qnil;  /* system error */
    if (len > RARRAY_LEN(stack)) len = RARRAY_LEN(stack);
    return rb_ary_new4(len, RARRAY_PTR(stack) + RARRAY_LEN(stack) - len);
}

static void
cut_stack_tail(VALUE stack, long len)
{
    while (len > 0) {
        rb_ary_pop(stack);
        len--;
    }
}

#define STACK_INIT_LEN 64
#define NEW_STACK() rb_ary_new2(STACK_INIT_LEN)
#define PUSH(s, i) rb_ary_store(s, RARRAY_LEN(s), i)
#define POP(s) rb_ary_pop(s)
#define LAST_I(s) \
    ((RARRAY_LEN(s) > 0) ? RARRAY_PTR(s)[RARRAY_LEN(s) - 1] : Qnil)
#define GET_TAIL(s, len) get_stack_tail(s, len)
#define CUT_TAIL(s, len) cut_stack_tail(s, len)

/* -----------------------------------------------------------------------
                       struct cparse_params
----------------------------------------------------------------------- */

struct cparse_params {
    VALUE value_v;         /* VALUE version of this struct */

    VALUE parser;          /* parser object */

    int   lex_is_iterator;
    VALUE lexer;           /* scanner object */
    ID    lexmid;          /* name of scanner method (must be an iterator) */

    /* State transition tables (immutable)
       Data structure is from Dragon Book 4.9 */
    /* action table */
    VALUE action_table;
    VALUE action_check;
    VALUE action_default;
    VALUE action_pointer;
    /* goto table */
    VALUE goto_table;
    VALUE goto_check;
    VALUE goto_default;
    VALUE goto_pointer;

    long  nt_base;         /* NonTerminal BASE index */
    VALUE reduce_table;    /* reduce data table */
    VALUE token_table;     /* token conversion table */

    /* parser stacks and parameters */
    VALUE state;
    long curstate;
    VALUE vstack;
    VALUE tstack;
    VALUE t;
    long shift_n;
    long reduce_n;
    long ruleno;

    long errstatus;         /* nonzero in error recovering mode */
    long nerr;              /* number of error */

    int use_result_var;

    VALUE retval;           /* return value of parser routine */
    long fin;               /* parse result status */
#define CP_FIN_ACCEPT  1
#define CP_FIN_EOT     2
#define CP_FIN_CANTPOP 3

    int debug;              /* user level debug */
    int sys_debug;          /* system level debug */

    long i;                 /* table index */
};

/* -----------------------------------------------------------------------
                        Parser Main Routines
----------------------------------------------------------------------- */

static VALUE racc_cparse _((VALUE parser, VALUE arg, VALUE sysdebug));
static VALUE racc_yyparse _((VALUE parser, VALUE lexer, VALUE lexmid,
                             VALUE arg, VALUE sysdebug));

static void call_lexer _((struct cparse_params *v));
static VALUE lexer_i _((VALUE block_args, VALUE data, VALUE self));

static VALUE assert_array _((VALUE a));
static long assert_integer _((VALUE n));
static VALUE assert_hash _((VALUE h));
static VALUE initialize_params _((VALUE vparams, VALUE parser, VALUE arg,
                                 VALUE lexer, VALUE lexmid));
static void cparse_params_mark _((void *ptr));

static void parse_main _((struct cparse_params *v,
                         VALUE tok, VALUE val, int resume));
static void extract_user_token _((struct cparse_params *v,
                                  VALUE block_args, VALUE *tok, VALUE *val));
static void shift _((struct cparse_params* v, long act, VALUE tok, VALUE val));
static int reduce _((struct cparse_params* v, long act));
static VALUE reduce0 _((VALUE block_args, VALUE data, VALUE self));

#ifdef DEBUG
# define D_puts(msg)        if (v->sys_debug) puts(msg)
# define D_printf(fmt,arg)  if (v->sys_debug) printf(fmt,arg)
#else
# define D_puts(msg)
# define D_printf(fmt,arg)
#endif

static VALUE
racc_cparse(VALUE parser, VALUE arg, VALUE sysdebug)
{
    volatile VALUE vparams;
    struct cparse_params *v;

    vparams = Data_Make_Struct(CparseParams, struct cparse_params,
                               cparse_params_mark, -1, v);
    D_puts("starting cparse");
    v->sys_debug = RTEST(sysdebug);
    vparams = initialize_params(vparams, parser, arg, Qnil, Qnil);
    v->lex_is_iterator = FALSE;
    parse_main(v, Qnil, Qnil, 0);

    return v->retval;
}

static VALUE
racc_yyparse(VALUE parser, VALUE lexer, VALUE lexmid, VALUE arg, VALUE sysdebug)
{
    volatile VALUE vparams;
    struct cparse_params *v;

    vparams = Data_Make_Struct(CparseParams, struct cparse_params,
                               cparse_params_mark, -1, v);
    v->sys_debug = RTEST(sysdebug);
    D_puts("start C yyparse");
    vparams = initialize_params(vparams, parser, arg, lexer, lexmid);
    v->lex_is_iterator = TRUE;
    D_puts("params initialized");
    parse_main(v, Qnil, Qnil, 0);
    call_lexer(v);
    if (!v->fin) {
        rb_raise(rb_eArgError, "%s() is finished before EndOfToken",
                 rb_id2name(v->lexmid));
    }

    return v->retval;
}

#ifdef HAVE_RB_BLOCK_CALL
static void
call_lexer(struct cparse_params *v)
{
    rb_block_call(v->lexer, v->lexmid, 0, NULL, lexer_i, v->value_v);
}
#else
static VALUE
lexer_iter(VALUE data)
{
    struct cparse_params *v;

    Data_Get_Struct(data, struct cparse_params, v);
    rb_funcall(v->lexer, v->lexmid, 0);
    return Qnil;
}

static void
call_lexer(struct cparse_params *v)
{
    rb_iterate(lexer_iter, v->value_v, lexer_i, v->value_v);
}
#endif

static VALUE
lexer_i(VALUE block_args, VALUE data, VALUE self)
{
    struct cparse_params *v;
    VALUE tok, val;

    Data_Get_Struct(data, struct cparse_params, v);
    if (v->fin)
        rb_raise(rb_eArgError, "extra token after EndOfToken");
    extract_user_token(v, block_args, &tok, &val);
    parse_main(v, tok, val, 1);
    if (v->fin && v->fin != CP_FIN_ACCEPT)
       rb_iter_break();
    return Qnil;
}

static VALUE
assert_array(VALUE a)
{
    Check_Type(a, T_ARRAY);
    return a;
}

static VALUE
assert_hash(VALUE h)
{
    Check_Type(h, T_HASH);
    return h;
}

static long
assert_integer(VALUE n)
{
    return NUM2LONG(n);
}

static VALUE
initialize_params(VALUE vparams, VALUE parser, VALUE arg, VALUE lexer, VALUE lexmid)
{
    struct cparse_params *v;

    Data_Get_Struct(vparams, struct cparse_params, v);
    v->value_v = vparams;
    v->parser = parser;
    v->lexer = lexer;
    if (! NIL_P(lexmid))
        v->lexmid = value_to_id(lexmid);

    v->debug = RTEST(rb_ivar_get(parser, id_yydebug));

    Check_Type(arg, T_ARRAY);
    if (!(13 <= RARRAY_LEN(arg) && RARRAY_LEN(arg) <= 14))
        rb_raise(RaccBug, "[Racc Bug] wrong arg.size %ld", RARRAY_LEN(arg));
    v->action_table   = assert_array  (RARRAY_PTR(arg)[ 0]);
    v->action_check   = assert_array  (RARRAY_PTR(arg)[ 1]);
    v->action_default = assert_array  (RARRAY_PTR(arg)[ 2]);
    v->action_pointer = assert_array  (RARRAY_PTR(arg)[ 3]);
    v->goto_table     = assert_array  (RARRAY_PTR(arg)[ 4]);
    v->goto_check     = assert_array  (RARRAY_PTR(arg)[ 5]);
    v->goto_default   = assert_array  (RARRAY_PTR(arg)[ 6]);
    v->goto_pointer   = assert_array  (RARRAY_PTR(arg)[ 7]);
    v->nt_base        = assert_integer(RARRAY_PTR(arg)[ 8]);
    v->reduce_table   = assert_array  (RARRAY_PTR(arg)[ 9]);
    v->token_table    = assert_hash   (RARRAY_PTR(arg)[10]);
    v->shift_n        = assert_integer(RARRAY_PTR(arg)[11]);
    v->reduce_n       = assert_integer(RARRAY_PTR(arg)[12]);
    if (RARRAY_LEN(arg) > 13) {
        v->use_result_var = RTEST(RARRAY_PTR(arg)[13]);
    }
    else {
        v->use_result_var = TRUE;
    }

    v->tstack = v->debug ? NEW_STACK() : Qnil;
    v->vstack = NEW_STACK();
    v->state = NEW_STACK();
    v->curstate = 0;
    PUSH(v->state, INT2FIX(0));
    v->t = INT2FIX(FINAL_TOKEN + 1);   /* must not init to FINAL_TOKEN */
    v->nerr = 0;
    v->errstatus = 0;
    rb_ivar_set(parser, id_errstatus, LONG2NUM(v->errstatus));

    v->retval = Qnil;
    v->fin = 0;

    v->lex_is_iterator = FALSE;

    rb_iv_set(parser, "@vstack", v->vstack);
    if (v->debug) {
        rb_iv_set(parser, "@tstack", v->tstack);
    }
    else {
        rb_iv_set(parser, "@tstack", Qnil);
    }

    return vparams;
}

static void
cparse_params_mark(void *ptr)
{
    struct cparse_params *v = (struct cparse_params*)ptr;

    rb_gc_mark(v->value_v);
    rb_gc_mark(v->parser);
    rb_gc_mark(v->lexer);
    rb_gc_mark(v->action_table);
    rb_gc_mark(v->action_check);
    rb_gc_mark(v->action_default);
    rb_gc_mark(v->action_pointer);
    rb_gc_mark(v->goto_table);
    rb_gc_mark(v->goto_check);
    rb_gc_mark(v->goto_default);
    rb_gc_mark(v->goto_pointer);
    rb_gc_mark(v->reduce_table);
    rb_gc_mark(v->token_table);
    rb_gc_mark(v->state);
    rb_gc_mark(v->vstack);
    rb_gc_mark(v->tstack);
    rb_gc_mark(v->t);
    rb_gc_mark(v->retval);
}

static void
extract_user_token(struct cparse_params *v, VALUE block_args,
                   VALUE *tok, VALUE *val)
{
    if (NIL_P(block_args)) {
        /* EOF */
        *tok = Qfalse;
        *val = rb_str_new("$", 1);
        return;
    }

    if (TYPE(block_args) != T_ARRAY) {
        rb_raise(rb_eTypeError,
                 "%s() %s %s (must be Array[2])",
                 v->lex_is_iterator ? rb_id2name(v->lexmid) : "next_token",
                 v->lex_is_iterator ? "yielded" : "returned",
                 rb_class2name(CLASS_OF(block_args)));
    }
    if (RARRAY_LEN(block_args) != 2) {
        rb_raise(rb_eArgError,
                 "%s() %s wrong size of array (%ld for 2)",
                 v->lex_is_iterator ? rb_id2name(v->lexmid) : "next_token",
                 v->lex_is_iterator ? "yielded" : "returned",
                 RARRAY_LEN(block_args));
    }
    *tok = AREF(block_args, 0);
    *val = AREF(block_args, 1);
}

#define SHIFT(v,act,tok,val) shift(v,act,tok,val)
#define REDUCE(v,act) do {\
    switch (reduce(v,act)) {  \
      case 0: /* normal */    \
        break;                \
      case 1: /* yyerror */   \
        goto user_yyerror;    \
      case 2: /* yyaccept */  \
        D_puts("u accept");   \
        goto accept;          \
      default:                \
        break;                \
    }                         \
} while (0)

static void
parse_main(struct cparse_params *v, VALUE tok, VALUE val, int resume)
{
    long i;              /* table index */
    long act;            /* action type */
    VALUE act_value;     /* action type, VALUE version */
    int read_next = 1;   /* true if we need to read next token */
    VALUE tmp;

    if (resume)
        goto resume;

    while (1) {
        D_puts("");
        D_puts("---- enter new loop ----");
        D_puts("");

        D_printf("(act) k1=%ld\n", v->curstate);
        tmp = AREF(v->action_pointer, v->curstate);
        if (NIL_P(tmp)) goto notfound;
        D_puts("(act) pointer[k1] ok");
        i = NUM2LONG(tmp);

        D_printf("read_next=%d\n", read_next);
        if (read_next && (v->t != vFINAL_TOKEN)) {
            if (v->lex_is_iterator) {
                D_puts("resuming...");
                if (v->fin) rb_raise(rb_eArgError, "token given after EOF");
                v->i = i;  /* save i */
                return;
              resume:
                D_puts("resumed");
                i = v->i;  /* load i */
            }
            else {
                D_puts("next_token");
                tmp = rb_funcall(v->parser, id_nexttoken, 0);
                extract_user_token(v, tmp, &tok, &val);
            }
            /* convert token */
            v->t = rb_hash_aref(v->token_table, tok);
            if (NIL_P(v->t)) {
                v->t = vERROR_TOKEN;
            }
            D_printf("(act) t(k2)=%ld\n", NUM2LONG(v->t));
            if (v->debug) {
                rb_funcall(v->parser, id_d_read_token,
                           3, v->t, tok, val);
            }
        }
        read_next = 0;

        i += NUM2LONG(v->t);
        D_printf("(act) i=%ld\n", i);
        if (i < 0) goto notfound;

        act_value = AREF(v->action_table, i);
        if (NIL_P(act_value)) goto notfound;
        act = NUM2LONG(act_value);
        D_printf("(act) table[i]=%ld\n", act);

        tmp = AREF(v->action_check, i);
        if (NIL_P(tmp)) goto notfound;
        if (NUM2LONG(tmp) != v->curstate) goto notfound;
        D_printf("(act) check[i]=%ld\n", NUM2LONG(tmp));

        D_puts("(act) found");
      act_fixed:
        D_printf("act=%ld\n", act);
        goto handle_act;

      notfound:
        D_puts("(act) not found: use default");
        act_value = AREF(v->action_default, v->curstate);
        act = NUM2LONG(act_value);
        goto act_fixed;


      handle_act:
        if (act > 0 && act < v->shift_n) {
            D_puts("shift");
            if (v->errstatus > 0) {
                v->errstatus--;
                rb_ivar_set(v->parser, id_errstatus, LONG2NUM(v->errstatus));
            }
            SHIFT(v, act, v->t, val);
            read_next = 1;
        }
        else if (act < 0 && act > -(v->reduce_n)) {
            D_puts("reduce");
            REDUCE(v, act);
        }
        else if (act == -(v->reduce_n)) {
            goto error;
          error_recovered:
            ;   /* goto label requires stmt */
        }
        else if (act == v->shift_n) {
            D_puts("accept");
            goto accept;
        }
        else {
            rb_raise(RaccBug, "[Racc Bug] unknown act value %ld", act);
        }

        if (v->debug) {
            rb_funcall(v->parser, id_d_next_state,
                       2, LONG2NUM(v->curstate), v->state);
        }
    }
    /* not reach */


  accept:
    if (v->debug) rb_funcall(v->parser, id_d_accept, 0);
    v->retval = RARRAY_PTR(v->vstack)[0];
    v->fin = CP_FIN_ACCEPT;
    return;


  error:
    D_printf("error detected, status=%ld\n", v->errstatus);
    if (v->errstatus == 0) {
        v->nerr++;
        rb_funcall(v->parser, id_onerror,
                   3, v->t, val, v->vstack);
    }
  user_yyerror:
    if (v->errstatus == 3) {
        if (v->t == vFINAL_TOKEN) {
            v->retval = Qfalse;
            v->fin = CP_FIN_EOT;
            return;
        }
        read_next = 1;
    }
    v->errstatus = 3;
    rb_ivar_set(v->parser, id_errstatus, LONG2NUM(v->errstatus));

    /* check if we can shift/reduce error token */
    D_printf("(err) k1=%ld\n", v->curstate);
    D_printf("(err) k2=%d (error)\n", ERROR_TOKEN);
    while (1) {
        tmp = AREF(v->action_pointer, v->curstate);
        if (NIL_P(tmp)) goto error_pop;
        D_puts("(err) pointer[k1] ok");

        i = NUM2LONG(tmp) + ERROR_TOKEN;
        D_printf("(err) i=%ld\n", i);
        if (i < 0) goto error_pop;

        act_value = AREF(v->action_table, i);
        if (NIL_P(act_value)) {
            D_puts("(err) table[i] == nil");
            goto error_pop;
        }
        act = NUM2LONG(act_value);
        D_printf("(err) table[i]=%ld\n", act);

        tmp = AREF(v->action_check, i);
        if (NIL_P(tmp)) {
            D_puts("(err) check[i] == nil");
            goto error_pop;
        }
        if (NUM2LONG(tmp) != v->curstate) {
            D_puts("(err) check[i] != k1");
            goto error_pop;
        }

        D_puts("(err) found: can handle error token");
        break;

      error_pop:
        D_puts("(err) act not found: can't handle error token; pop");

        if (RARRAY_LEN(v->state) <= 1) {
            v->retval = Qnil;
            v->fin = CP_FIN_CANTPOP;
            return;
        }
        POP(v->state);
        POP(v->vstack);
        v->curstate = num_to_long(LAST_I(v->state));
        if (v->debug) {
            POP(v->tstack);
            rb_funcall(v->parser, id_d_e_pop,
                       3, v->state, v->tstack, v->vstack);
        }
    }

    /* shift/reduce error token */
    if (act > 0 && act < v->shift_n) {
        D_puts("e shift");
        SHIFT(v, act, ERROR_TOKEN, val);
    }
    else if (act < 0 && act > -(v->reduce_n)) {
        D_puts("e reduce");
        REDUCE(v, act);
    }
    else if (act == v->shift_n) {
        D_puts("e accept");
        goto accept;
    }
    else {
        rb_raise(RaccBug, "[Racc Bug] unknown act value %ld", act);
    }
    goto error_recovered;
}

static void
shift(struct cparse_params *v, long act, VALUE tok, VALUE val)
{
    PUSH(v->vstack, val);
    if (v->debug) {
        PUSH(v->tstack, tok);
        rb_funcall(v->parser, id_d_shift,
                   3, tok, v->tstack, v->vstack);
    }
    v->curstate = act;
    PUSH(v->state, LONG2NUM(v->curstate));
}

static int
reduce(struct cparse_params *v, long act)
{
    VALUE code;
    v->ruleno = -act * 3;
    code = rb_catch("racc_jump", reduce0, v->value_v);
    v->errstatus = num_to_long(rb_ivar_get(v->parser, id_errstatus));
    return NUM2INT(code);
}

static VALUE
reduce0(VALUE val, VALUE data, VALUE self)
{
    struct cparse_params *v;
    VALUE reduce_to, reduce_len, method_id;
    long len;
    ID mid;
    VALUE tmp, tmp_t = Qundef, tmp_v = Qundef;
    long i, k1, k2;
    VALUE goto_state;

    Data_Get_Struct(data, struct cparse_params, v);
    reduce_len = RARRAY_PTR(v->reduce_table)[v->ruleno];
    reduce_to  = RARRAY_PTR(v->reduce_table)[v->ruleno+1];
    method_id  = RARRAY_PTR(v->reduce_table)[v->ruleno+2];
    len = NUM2LONG(reduce_len);
    mid = value_to_id(method_id);

    /* call action */
    if (len == 0) {
        tmp = Qnil;
        if (mid != id_noreduce)
            tmp_v = rb_ary_new();
        if (v->debug)
            tmp_t = rb_ary_new();
    }
    else {
        if (mid != id_noreduce) {
            tmp_v = GET_TAIL(v->vstack, len);
            tmp = RARRAY_PTR(tmp_v)[0];
        }
        else {
            tmp = RARRAY_PTR(v->vstack)[ RARRAY_LEN(v->vstack) - len ];
        }
        CUT_TAIL(v->vstack, len);
        if (v->debug) {
            tmp_t = GET_TAIL(v->tstack, len);
            CUT_TAIL(v->tstack, len);
        }
        CUT_TAIL(v->state, len);
    }
    if (mid != id_noreduce) {
        if (v->use_result_var) {
            tmp = rb_funcall(v->parser, mid,
                             3, tmp_v, v->vstack, tmp);
        }
        else {
            tmp = rb_funcall(v->parser, mid,
                             2, tmp_v, v->vstack);
        }
    }

    /* then push result */
    PUSH(v->vstack, tmp);
    if (v->debug) {
        PUSH(v->tstack, reduce_to);
        rb_funcall(v->parser, id_d_reduce,
                   4, tmp_t, reduce_to, v->tstack, v->vstack);
    }

    /* calculate transition state */
    if (RARRAY_LEN(v->state) == 0)
        rb_raise(RaccBug, "state stack unexpectedly empty");
    k2 = num_to_long(LAST_I(v->state));
    k1 = num_to_long(reduce_to) - v->nt_base;
    D_printf("(goto) k1=%ld\n", k1);
    D_printf("(goto) k2=%ld\n", k2);

    tmp = AREF(v->goto_pointer, k1);
    if (NIL_P(tmp)) goto notfound;

    i = NUM2LONG(tmp) + k2;
    D_printf("(goto) i=%ld\n", i);
    if (i < 0) goto notfound;

    goto_state = AREF(v->goto_table, i);
    if (NIL_P(goto_state)) {
        D_puts("(goto) table[i] == nil");
        goto notfound;
    }
    D_printf("(goto) table[i]=%ld (goto_state)\n", NUM2LONG(goto_state));

    tmp = AREF(v->goto_check, i);
    if (NIL_P(tmp)) {
        D_puts("(goto) check[i] == nil");
        goto notfound;
    }
    if (tmp != LONG2NUM(k1)) {
        D_puts("(goto) check[i] != table[i]");
        goto notfound;
    }
    D_printf("(goto) check[i]=%ld\n", NUM2LONG(tmp));

    D_puts("(goto) found");
  transit:
    PUSH(v->state, goto_state);
    v->curstate = NUM2LONG(goto_state);
    return INT2FIX(0);

  notfound:
    D_puts("(goto) not found: use default");
    /* overwrite `goto-state' by default value */
    goto_state = AREF(v->goto_default, k1);
    goto transit;
}

/* -----------------------------------------------------------------------
                          Ruby Interface
----------------------------------------------------------------------- */

void
Init_cparse(void)
{
    VALUE Racc, Parser;
    ID id_racc = rb_intern("Racc");

    if (rb_const_defined(rb_cObject, id_racc)) {
        Racc = rb_const_get(rb_cObject, id_racc);
        Parser = rb_const_get_at(Racc, rb_intern("Parser"));
    }
    else {
        Racc = rb_define_module("Racc");
        Parser = rb_define_class_under(Racc, "Parser", rb_cObject);
    }
    rb_define_private_method(Parser, "_racc_do_parse_c", racc_cparse, 2);
    rb_define_private_method(Parser, "_racc_yyparse_c", racc_yyparse, 4);
    rb_define_const(Parser, "Racc_Runtime_Core_Version_C",
                    rb_str_new2(RACC_VERSION));
    rb_define_const(Parser, "Racc_Runtime_Core_Id_C",
        rb_str_new2("$originalId: cparse.c,v 1.8 2006/07/06 11:39:46 aamine Exp $"));

    CparseParams = rb_define_class_under(Racc, "CparseParams", rb_cObject);

    RaccBug = rb_eRuntimeError;

    id_yydebug      = rb_intern("@yydebug");
    id_nexttoken    = rb_intern("next_token");
    id_onerror      = rb_intern("on_error");
    id_noreduce     = rb_intern("_reduce_none");
    id_errstatus    = rb_intern("@racc_error_status");

    id_d_shift       = rb_intern("racc_shift");
    id_d_reduce      = rb_intern("racc_reduce");
    id_d_accept      = rb_intern("racc_accept");
    id_d_read_token  = rb_intern("racc_read_token");
    id_d_next_state  = rb_intern("racc_next_state");
    id_d_e_pop       = rb_intern("racc_e_pop");
}
