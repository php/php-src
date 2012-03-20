/************************************************

  tkutil.c -

  $Author$
  created at: Fri Nov  3 00:47:54 JST 1995

************************************************/

#define TKUTIL_RELEASE_DATE "2010-03-26"

#include "ruby.h"

#ifdef RUBY_VM
static int rb_thread_critical; /* dummy */
#else
/* On Ruby 1.8.x, use rb_thread_critical (defined at rubysig.h) */
#include "rubysig.h"
#endif
#ifdef HAVE_RUBY_ST_H
#include "ruby/st.h"
#else
#include "st.h"
#endif

#if !defined(RHASH_TBL)
#define RHASH_TBL(h) (RHASH(h)->tbl)
#endif
#if !defined(RSTRING_PTR)
#define RSTRING_PTR(s) (RSTRING(s)->ptr)
#define RSTRING_LEN(s) (RSTRING(s)->len)
#endif
#if !defined(RARRAY_PTR)
#define RARRAY_PTR(s) (RARRAY(s)->ptr)
#define RARRAY_LEN(s) (RARRAY(s)->len)
#endif

#if defined(HAVE_STRNDUP) && !defined(_GNU_SOURCE)
extern char *strndup(const char* _ptr, size_t _len);
#endif

static VALUE cMethod;

static VALUE cTclTkLib;

static VALUE cTkObject;
static VALUE cTkCallbackEntry;

static VALUE TK_None;

static VALUE cCB_SUBST;
static VALUE cSUBST_INFO;

static VALUE ENCODING_NAME_UTF8; /* for saving GC cost */

static ID ID_split_tklist;
static ID ID_toUTF8;
static ID ID_fromUTF8;
static ID ID_path;
static ID ID_at_path;
static ID ID_at_enc;
static ID ID_to_eval;
static ID ID_to_s;
static ID ID_source;
static ID ID_downcase;
static ID ID_install_cmd;
static ID ID_merge_tklist;
static ID ID_encoding;
static ID ID_encoding_system;
static ID ID_call;

static ID ID_SUBST_INFO;

static VALUE CALLBACK_TABLE;
static unsigned long CALLBACK_ID_NUM = 0;

/*************************************/

#if defined(HAVE_RB_OBJ_INSTANCE_EXEC) && !defined(RUBY_VM)
extern VALUE rb_obj_instance_exec _((int, VALUE*, VALUE));
#endif
static VALUE
tk_s_new(argc, argv, klass)
    int argc;
    VALUE *argv;
    VALUE klass;
{
    VALUE obj = rb_class_new_instance(argc, argv, klass);

    if (rb_block_given_p()) {
#ifndef HAVE_RB_OBJ_INSTANCE_EXEC
      rb_obj_instance_eval(0, 0, obj);
#else
      rb_obj_instance_exec(1, &obj, obj);
#endif
    }
    return obj;
}

/*************************************/

static VALUE
tkNone_to_s(self)
    VALUE self;
{
    return rb_str_new2("");
}

static VALUE
tkNone_inspect(self)
    VALUE self;
{
    return rb_str_new2("None");
}

/*************************************/

static VALUE
tk_obj_untrust(self, obj)
    VALUE self;
    VALUE obj;
{
#ifdef HAVE_RB_OBJ_TAINT
  rb_obj_taint(obj);
#endif
#ifdef HAVE_RB_OBJ_UNTRUST
  rb_obj_untrust(obj);
#endif

  return obj;
}

static VALUE
tk_eval_cmd(argc, argv, self)
    int argc;
    VALUE argv[];
    VALUE self;
{
    volatile VALUE cmd, rest;

    rb_scan_args(argc, argv, "1*", &cmd, &rest);
    return rb_eval_cmd(cmd, rest, 0);
}

static VALUE
tk_do_callback(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
#if 0
    volatile VALUE id;
    volatile VALUE rest;

    rb_scan_args(argc, argv, "1*", &id, &rest);
    return rb_apply(rb_hash_aref(CALLBACK_TABLE, id), ID_call, rest);
#endif
    return rb_funcall2(rb_hash_aref(CALLBACK_TABLE, argv[0]),
                       ID_call, argc - 1, argv + 1);
}

static const char cmd_id_head[] = "ruby_cmd TkUtil callback ";
static const char cmd_id_prefix[] = "cmd";

static VALUE
tk_install_cmd_core(cmd)
    VALUE cmd;
{
    volatile VALUE id_num;

    id_num = ULONG2NUM(CALLBACK_ID_NUM++);
    id_num = rb_funcall(id_num, ID_to_s, 0, 0);
    id_num = rb_str_append(rb_str_new2(cmd_id_prefix), id_num);
    rb_hash_aset(CALLBACK_TABLE, id_num, cmd);
    return rb_str_append(rb_str_new2(cmd_id_head), id_num);
}

static VALUE
tk_install_cmd(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    volatile VALUE cmd;

#if 0
    if (rb_scan_args(argc, argv, "01", &cmd) == 0) {
        cmd = rb_block_proc();
    }
    return tk_install_cmd_core(cmd);
#endif
    if (argc == 0) {
        cmd = rb_block_proc();
    } else {
        cmd = argv[0];
    }
    return tk_install_cmd_core(cmd);
}

static VALUE
tk_uninstall_cmd(self, cmd_id)
    VALUE self;
    VALUE cmd_id;
{
    size_t head_len = strlen(cmd_id_head);
    size_t prefix_len = strlen(cmd_id_prefix);

    StringValue(cmd_id);
    if (strncmp(cmd_id_head, RSTRING_PTR(cmd_id), head_len) != 0) {
        return Qnil;
    }
    if (strncmp(cmd_id_prefix,
                RSTRING_PTR(cmd_id) + head_len, prefix_len) != 0) {
        return Qnil;
    }

    return rb_hash_delete(CALLBACK_TABLE,
                          rb_str_new2(RSTRING_PTR(cmd_id) + head_len));
}

static VALUE
tk_toUTF8(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    return rb_funcall2(cTclTkLib, ID_toUTF8, argc, argv);
}

static VALUE
tk_fromUTF8(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    return rb_funcall2(cTclTkLib, ID_fromUTF8, argc, argv);
}

static VALUE
fromDefaultEnc_toUTF8(str, self)
    VALUE str;
    VALUE self;
{
    VALUE argv[1];

    argv[0] = str;
    return tk_toUTF8(1, argv, self);
}

#if 0
static VALUE
fromUTF8_toDefaultEnc(str, self)
    VALUE str;
    VALUE self;
{
    VALUE argv[1];

    argv[0] = str;
    return tk_fromUTF8(1, argv, self);
}
#endif

static int
to_strkey(key, value, hash)
    VALUE key;
    VALUE value;
    VALUE hash;
{
    rb_hash_aset(hash, rb_funcall(key, ID_to_s, 0, 0), value);
    return ST_CHECK;
}

static VALUE
tk_symbolkey2str(self, keys)
    VALUE self;
    VALUE keys;
{
    volatile VALUE new_keys = rb_hash_new();

    if NIL_P(keys) return new_keys;
    keys = rb_convert_type(keys, T_HASH, "Hash", "to_hash");
    st_foreach_check(RHASH_TBL(keys), to_strkey, new_keys, Qundef);
    return new_keys;
}

static VALUE get_eval_string_core _((VALUE, VALUE, VALUE));
static VALUE ary2list _((VALUE, VALUE, VALUE));
static VALUE ary2list2 _((VALUE, VALUE, VALUE));
static VALUE hash2list _((VALUE, VALUE));
static VALUE hash2list_enc _((VALUE, VALUE));
static VALUE hash2kv _((VALUE, VALUE, VALUE));
static VALUE hash2kv_enc _((VALUE, VALUE, VALUE));

static VALUE
ary2list(ary, enc_flag, self)
    VALUE ary;
    VALUE enc_flag;
    VALUE self;
{
    long idx, idx2, size, size2;
    int req_chk_flag;
    volatile VALUE val, val2, str_val;
    volatile VALUE dst;
    volatile VALUE sys_enc, dst_enc, str_enc;

    sys_enc = rb_funcall(cTclTkLib, ID_encoding, 0, 0);
    if (NIL_P(sys_enc)) {
      sys_enc = rb_funcall(cTclTkLib, ID_encoding_system, 0, 0);
      sys_enc = rb_funcall(sys_enc, ID_to_s, 0, 0);
    }

    if NIL_P(enc_flag) {
        dst_enc = sys_enc;
        req_chk_flag = 1;
    } else if (TYPE(enc_flag) == T_TRUE || TYPE(enc_flag) == T_FALSE) {
        dst_enc = enc_flag;
        req_chk_flag = 0;
    } else {
        dst_enc = rb_funcall(enc_flag, ID_to_s, 0, 0);
        req_chk_flag = 0;
    }

    /* size = RARRAY_LEN(ary); */
    size = 0;
    for(idx = 0; idx < RARRAY_LEN(ary); idx++) {
        if (TYPE(RARRAY_PTR(ary)[idx]) == T_HASH) {
            size += 2 * RHASH_SIZE(RARRAY_PTR(ary)[idx]);
        } else {
            size++;
        }
    }

    dst = rb_ary_new2(size);
    for(idx = 0; idx < RARRAY_LEN(ary); idx++) {
        val = RARRAY_PTR(ary)[idx];
        str_val = Qnil;
        switch(TYPE(val)) {
        case T_ARRAY:
            str_val = ary2list(val, enc_flag, self);
            rb_ary_push(dst, str_val);

            if (req_chk_flag) {
                str_enc = rb_ivar_get(str_val, ID_at_enc);
                if (!NIL_P(str_enc)) {
                    str_enc = rb_funcall(str_enc, ID_to_s, 0, 0);
                } else {
                    str_enc = sys_enc;
                }
                if (!rb_str_cmp(str_enc, dst_enc)) {
                    dst_enc = Qtrue;
                    req_chk_flag = 0;
                }
            }

            break;

        case T_HASH:
	    /* rb_ary_push(dst, hash2list(val, self)); */
            if (RTEST(enc_flag)) {
                val = hash2kv_enc(val, Qnil, self);
            } else {
                val = hash2kv(val, Qnil, self);
            }
            size2 = RARRAY_LEN(val);
            for(idx2 = 0; idx2 < size2; idx2++) {
                val2 = RARRAY_PTR(val)[idx2];
                switch(TYPE(val2)) {
                case T_ARRAY:
                    str_val = ary2list(val2, enc_flag, self);
                    rb_ary_push(dst, str_val);
                    break;

                case T_HASH:
                    if (RTEST(enc_flag)) {
                        str_val = hash2list_enc(val2, self);
                    } else {
                        str_val = hash2list(val2, self);
                    }
                    rb_ary_push(dst, str_val);
                    break;

                default:
                    if (val2 != TK_None) {
                        str_val = get_eval_string_core(val2, enc_flag, self);
                        rb_ary_push(dst, str_val);
                    }
                }

                if (req_chk_flag) {
                    str_enc = rb_ivar_get(str_val, ID_at_enc);
                    if (!NIL_P(str_enc)) {
                        str_enc = rb_funcall(str_enc, ID_to_s, 0, 0);
                    } else {
                        str_enc = sys_enc;
                    }
                    if (!rb_str_cmp(str_enc, dst_enc)) {
                        dst_enc = Qtrue;
                        req_chk_flag = 0;
                    }
                }
            }
            break;

        default:
            if (val != TK_None) {
                str_val = get_eval_string_core(val, enc_flag, self);
                rb_ary_push(dst, str_val);

                if (req_chk_flag) {
                    str_enc = rb_ivar_get(str_val, ID_at_enc);
                    if (!NIL_P(str_enc)) {
                        str_enc = rb_funcall(str_enc, ID_to_s, 0, 0);
                    } else {
                        str_enc = sys_enc;
                    }
                    if (!rb_str_cmp(str_enc, dst_enc)) {
                        dst_enc = Qtrue;
                        req_chk_flag = 0;
                    }
                }
            }
        }
    }

    if (RTEST(dst_enc) && !NIL_P(sys_enc)) {
        for(idx = 0; idx < RARRAY_LEN(dst); idx++) {
            str_val = RARRAY_PTR(dst)[idx];
            if (rb_obj_respond_to(self, ID_toUTF8, Qtrue)) {
                str_val = rb_funcall(self, ID_toUTF8, 1, str_val);
            } else {
                str_val = rb_funcall(cTclTkLib, ID_toUTF8, 1, str_val);
            }
            RARRAY_PTR(dst)[idx] = str_val;
        }
        val = rb_apply(cTclTkLib, ID_merge_tklist, dst);
        if (TYPE(dst_enc) == T_STRING) {
            val = rb_funcall(cTclTkLib, ID_fromUTF8, 2, val, dst_enc);
            rb_ivar_set(val, ID_at_enc, dst_enc);
        } else {
            rb_ivar_set(val, ID_at_enc, ENCODING_NAME_UTF8);
        }
        return val;
    } else {
        return rb_apply(cTclTkLib, ID_merge_tklist, dst);
    }
}

static VALUE
ary2list2(ary, enc_flag, self)
    VALUE ary;
    VALUE enc_flag;
    VALUE self;
{
    long idx, size;
    int req_chk_flag;
    volatile VALUE val, str_val;
    volatile VALUE dst;
    volatile VALUE sys_enc, dst_enc, str_enc;

    sys_enc = rb_funcall(cTclTkLib, ID_encoding, 0, 0);
    if NIL_P(sys_enc) {
      sys_enc = rb_funcall(cTclTkLib, ID_encoding_system, 0, 0);
      sys_enc = rb_funcall(sys_enc, ID_to_s, 0, 0);
    }

    if NIL_P(enc_flag) {
        dst_enc = sys_enc;
        req_chk_flag = 1;
    } else if (TYPE(enc_flag) == T_TRUE || TYPE(enc_flag) == T_FALSE) {
        dst_enc = enc_flag;
        req_chk_flag = 0;
    } else {
        dst_enc = rb_funcall(enc_flag, ID_to_s, 0, 0);
        req_chk_flag = 0;
    }

    size = RARRAY_LEN(ary);
    dst = rb_ary_new2(size);
    for(idx = 0; idx < RARRAY_LEN(ary); idx++) {
        val = RARRAY_PTR(ary)[idx];
        str_val = Qnil;
        switch(TYPE(val)) {
        case T_ARRAY:
            str_val = ary2list(val, enc_flag, self);
            break;

        case T_HASH:
            if (RTEST(enc_flag)) {
                str_val = hash2list(val, self);
            } else {
                str_val = hash2list_enc(val, self);
            }
            break;

        default:
            if (val != TK_None) {
                str_val = get_eval_string_core(val, enc_flag, self);
            }
        }

        if (!NIL_P(str_val)) {
            rb_ary_push(dst, str_val);

            if (req_chk_flag) {
                str_enc = rb_ivar_get(str_val, ID_at_enc);
                if (!NIL_P(str_enc)) {
                    str_enc = rb_funcall(str_enc, ID_to_s, 0, 0);
                } else {
                    str_enc = sys_enc;
                }
                if (!rb_str_cmp(str_enc, dst_enc)) {
                    dst_enc = Qtrue;
                    req_chk_flag = 0;
                }
            }
        }
    }

    if (RTEST(dst_enc) && !NIL_P(sys_enc)) {
        for(idx = 0; idx < RARRAY_LEN(dst); idx++) {
            str_val = RARRAY_PTR(dst)[idx];
            if (rb_obj_respond_to(self, ID_toUTF8, Qtrue)) {
                str_val = rb_funcall(self, ID_toUTF8, 1, str_val);
            } else {
                str_val = rb_funcall(cTclTkLib, ID_toUTF8, 1, str_val);
            }
            RARRAY_PTR(dst)[idx] = str_val;
        }
        val = rb_apply(cTclTkLib, ID_merge_tklist, dst);
        if (TYPE(dst_enc) == T_STRING) {
            val = rb_funcall(cTclTkLib, ID_fromUTF8, 2, val, dst_enc);
            rb_ivar_set(val, ID_at_enc, dst_enc);
        } else {
            rb_ivar_set(val, ID_at_enc, ENCODING_NAME_UTF8);
        }
        return val;
    } else {
        return rb_apply(cTclTkLib, ID_merge_tklist, dst);
    }
}

static VALUE
key2keyname(key)
    VALUE key;
{
    return rb_str_append(rb_str_new2("-"), rb_funcall(key, ID_to_s, 0, 0));
}

static VALUE
assoc2kv(assoc, ary, self)
    VALUE assoc;
    VALUE ary;
    VALUE self;
{
    long i, j, len;
    volatile VALUE pair;
    volatile VALUE val;
    volatile VALUE dst = rb_ary_new2(2 * RARRAY_LEN(assoc));

    len = RARRAY_LEN(assoc);

    for(i = 0; i < len; i++) {
        pair = RARRAY_PTR(assoc)[i];
        if (TYPE(pair) != T_ARRAY) {
            rb_ary_push(dst, key2keyname(pair));
            continue;
        }
        switch(RARRAY_LEN(assoc)) {
        case 2:
            rb_ary_push(dst, RARRAY_PTR(pair)[2]);

        case 1:
            rb_ary_push(dst, key2keyname(RARRAY_PTR(pair)[0]));

        case 0:
            continue;

        default:
            rb_ary_push(dst, key2keyname(RARRAY_PTR(pair)[0]));

            val = rb_ary_new2(RARRAY_LEN(pair) - 1);
            for(j = 1; j < RARRAY_LEN(pair); j++) {
                rb_ary_push(val, RARRAY_PTR(pair)[j]);
            }

            rb_ary_push(dst, val);
        }
    }

    if (NIL_P(ary)) {
        return dst;
    } else {
        return rb_ary_plus(ary, dst);
    }
}

static VALUE
assoc2kv_enc(assoc, ary, self)
    VALUE assoc;
    VALUE ary;
    VALUE self;
{
    long i, j, len;
    volatile VALUE pair;
    volatile VALUE val;
    volatile VALUE dst = rb_ary_new2(2 * RARRAY_LEN(assoc));

    len = RARRAY_LEN(assoc);

    for(i = 0; i < len; i++) {
        pair = RARRAY_PTR(assoc)[i];
        if (TYPE(pair) != T_ARRAY) {
            rb_ary_push(dst, key2keyname(pair));
            continue;
        }
        switch(RARRAY_LEN(assoc)) {
        case 2:
            rb_ary_push(dst, get_eval_string_core(RARRAY_PTR(pair)[2], Qtrue, self));

        case 1:
            rb_ary_push(dst, key2keyname(RARRAY_PTR(pair)[0]));

        case 0:
            continue;

        default:
            rb_ary_push(dst, key2keyname(RARRAY_PTR(pair)[0]));

            val = rb_ary_new2(RARRAY_LEN(pair) - 1);
            for(j = 1; j < RARRAY_LEN(pair); j++) {
                rb_ary_push(val, RARRAY_PTR(pair)[j]);
            }

            rb_ary_push(dst, get_eval_string_core(val, Qtrue, self));
        }
    }

    if (NIL_P(ary)) {
        return dst;
    } else {
        return rb_ary_plus(ary, dst);
    }
}

static int
push_kv(key, val, args)
    VALUE key;
    VALUE val;
    VALUE args;
{
    volatile VALUE ary;

    ary = RARRAY_PTR(args)[0];

#if 0
    rb_ary_push(ary, key2keyname(key));
    if (val != TK_None) rb_ary_push(ary, val);
#endif
    rb_ary_push(ary, key2keyname(key));

    if (val == TK_None) return ST_CHECK;

    rb_ary_push(ary, get_eval_string_core(val, Qnil, RARRAY_PTR(args)[1]));

    return ST_CHECK;
}

static VALUE
hash2kv(hash, ary, self)
    VALUE hash;
    VALUE ary;
    VALUE self;
{
    volatile VALUE dst = rb_ary_new2(2 * RHASH_SIZE(hash));
    volatile VALUE args = rb_ary_new3(2, dst, self);

    st_foreach_check(RHASH_TBL(hash), push_kv, args, Qundef);

    if (NIL_P(ary)) {
        return dst;
    } else {
        return rb_ary_concat(ary, dst);
    }
}

static int
push_kv_enc(key, val, args)
    VALUE key;
    VALUE val;
    VALUE args;
{
    volatile VALUE ary;

    ary = RARRAY_PTR(args)[0];

#if 0
    rb_ary_push(ary, key2keyname(key));
    if (val != TK_None) {
        rb_ary_push(ary, get_eval_string_core(val, Qtrue,
                                              RARRAY_PTR(args)[1]));
    }
#endif
    rb_ary_push(ary, key2keyname(key));

    if (val == TK_None) return ST_CHECK;

    rb_ary_push(ary, get_eval_string_core(val, Qtrue, RARRAY_PTR(args)[1]));

    return ST_CHECK;
}

static VALUE
hash2kv_enc(hash, ary, self)
    VALUE hash;
    VALUE ary;
    VALUE self;
{
    volatile VALUE dst = rb_ary_new2(2 * RHASH_SIZE(hash));
    volatile VALUE args = rb_ary_new3(2, dst, self);

    st_foreach_check(RHASH_TBL(hash), push_kv_enc, args, Qundef);

    if (NIL_P(ary)) {
        return dst;
    } else {
        return rb_ary_concat(ary, dst);
    }
}

static VALUE
hash2list(hash, self)
    VALUE hash;
    VALUE self;
{
    return ary2list2(hash2kv(hash, Qnil, self), Qfalse, self);
}


static VALUE
hash2list_enc(hash, self)
    VALUE hash;
    VALUE self;
{
    return ary2list2(hash2kv_enc(hash, Qnil, self), Qfalse, self);
}

static VALUE
tk_hash_kv(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    volatile VALUE hash, enc_flag, ary;

    ary = Qnil;
    enc_flag = Qnil;
    switch(argc) {
    case 3:
        ary = argv[2];
    case 2:
        enc_flag = argv[1];
    case 1:
        hash = argv[0];
        break;
    case 0:
        rb_raise(rb_eArgError, "too few arguments");
    default: /* >= 3 */
        rb_raise(rb_eArgError, "too many arguments");
    }

    switch(TYPE(hash)) {
    case T_ARRAY:
        if (RTEST(enc_flag)) {
            return assoc2kv_enc(hash, ary, self);
        } else {
            return assoc2kv(hash, ary, self);
        }

    case T_HASH:
        if (RTEST(enc_flag)) {
            return hash2kv_enc(hash, ary, self);
        } else {
            return hash2kv(hash, ary, self);
        }

    case T_NIL:
        if (NIL_P(ary)) {
            return rb_ary_new();
        } else {
            return ary;
        }

    default:
        if (hash == TK_None) {
            if (NIL_P(ary)) {
                return rb_ary_new();
            } else {
                return ary;
            }
        }
        rb_raise(rb_eArgError, "Hash is expected for 1st argument");
    }
}

static VALUE
get_eval_string_core(obj, enc_flag, self)
    VALUE obj;
    VALUE enc_flag;
    VALUE self;
{
    switch(TYPE(obj)) {
    case T_FLOAT:
    case T_FIXNUM:
    case T_BIGNUM:
        return rb_funcall(obj, ID_to_s, 0, 0);

    case T_STRING:
        if (RTEST(enc_flag)) {
            if (rb_obj_respond_to(self, ID_toUTF8, Qtrue)) {
                return rb_funcall(self, ID_toUTF8, 1, obj);
            } else {
                return fromDefaultEnc_toUTF8(obj, self);
            }
        } else {
            return obj;
        }

    case T_SYMBOL:
        if (RTEST(enc_flag)) {
            if (rb_obj_respond_to(self, ID_toUTF8, Qtrue)) {
                return rb_funcall(self, ID_toUTF8, 1,
                                  rb_str_new2(rb_id2name(SYM2ID(obj))));
            } else {
                return fromDefaultEnc_toUTF8(rb_str_new2(rb_id2name(SYM2ID(obj))), self);
            }
        } else {
#ifdef HAVE_RB_SYM_TO_S
            return rb_sym_to_s(obj);
#else
            return rb_str_new2(rb_id2name(SYM2ID(obj)));
#endif
        }

    case T_HASH:
        if (RTEST(enc_flag)) {
            return hash2list_enc(obj, self);
        } else {
            return hash2list(obj, self);
        }

    case T_ARRAY:
        return ary2list(obj, enc_flag, self);

    case T_FALSE:
        return rb_str_new2("0");

    case T_TRUE:
        return rb_str_new2("1");

    case T_NIL:
        return rb_str_new2("");

    case T_REGEXP:
        return rb_funcall(obj, ID_source, 0, 0);

    default:
        if (rb_obj_is_kind_of(obj, cTkObject)) {
            /* return rb_str_new3(rb_funcall(obj, ID_path, 0, 0)); */
            return get_eval_string_core(rb_funcall(obj, ID_path, 0, 0),
                                        enc_flag, self);
        }

        if (rb_obj_is_kind_of(obj, rb_cProc)
            || rb_obj_is_kind_of(obj, cMethod)
            || rb_obj_is_kind_of(obj, cTkCallbackEntry)) {
            if (rb_obj_respond_to(self, ID_install_cmd, Qtrue)) {
                return rb_funcall(self, ID_install_cmd, 1, obj);
            } else {
                return tk_install_cmd_core(obj);
            }
        }

        if (obj == TK_None)  return Qnil;

        if (rb_obj_respond_to(obj, ID_to_eval, Qtrue)) {
            /* return rb_funcall(obj, ID_to_eval, 0, 0); */
            return get_eval_string_core(rb_funcall(obj, ID_to_eval, 0, 0),
                                        enc_flag, self);
        } else if (rb_obj_respond_to(obj, ID_path, Qtrue)) {
            /* return rb_funcall(obj, ID_path, 0, 0); */
            return get_eval_string_core(rb_funcall(obj, ID_path, 0, 0),
                                        enc_flag, self);
        } else if (rb_obj_respond_to(obj, ID_to_s, Qtrue)) {
            return rb_funcall(obj, ID_to_s, 0, 0);
        }
    }

    rb_warning("fail to convert '%s' to string for Tk",
               RSTRING_PTR(rb_funcall(obj, rb_intern("inspect"), 0, 0)));

    return obj;
}

static VALUE
tk_get_eval_string(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    volatile VALUE obj, enc_flag;

    if (rb_scan_args(argc, argv, "11", &obj, &enc_flag) == 1) {
        enc_flag = Qnil;
    }

    return get_eval_string_core(obj, enc_flag, self);
}

static VALUE
tk_get_eval_enc_str(self, obj)
    VALUE self;
    VALUE obj;
{
    if (obj == TK_None) {
        return obj;
    } else {
        return get_eval_string_core(obj, Qtrue, self);
    }
}

static VALUE
tk_conv_args(argc, argv, self)
    int   argc;
    VALUE *argv; /* [0]:base_array, [1]:enc_mode, [2]..[n]:args */
    VALUE self;
{
    int idx, size;
    volatile VALUE dst;
    int thr_crit_bup;
    VALUE old_gc;

    if (argc < 2) {
      rb_raise(rb_eArgError, "too few arguments");
    }

    thr_crit_bup = rb_thread_critical;
    rb_thread_critical = Qtrue;
    old_gc = rb_gc_disable();

    for(size = 0, idx = 2; idx < argc; idx++) {
        if (TYPE(argv[idx]) == T_HASH) {
            size += 2 * RHASH_SIZE(argv[idx]);
        } else {
            size++;
        }
    }
    /* dst = rb_ary_new2(argc - 2); */
    dst = rb_ary_new2(size);
    for(idx = 2; idx < argc; idx++) {
        if (TYPE(argv[idx]) == T_HASH) {
            if (RTEST(argv[1])) {
                hash2kv_enc(argv[idx], dst, self);
            } else {
                hash2kv(argv[idx], dst, self);
            }
        } else if (argv[idx] != TK_None) {
            rb_ary_push(dst, get_eval_string_core(argv[idx], argv[1], self));
        }
    }

    if (old_gc == Qfalse) rb_gc_enable();
    rb_thread_critical = thr_crit_bup;

    return rb_ary_plus(argv[0], dst);
}


/*************************************/

static VALUE
tcl2rb_bool(self, value)
    VALUE self;
    VALUE value;
{
    if (TYPE(value) == T_FIXNUM) {
        if (NUM2INT(value) == 0) {
            return Qfalse;
        } else {
            return Qtrue;
        }
    }

    if (TYPE(value) == T_TRUE || TYPE(value) == T_FALSE) {
        return value;
    }

    rb_check_type(value, T_STRING);

    value = rb_funcall(value, ID_downcase, 0);

    if (RSTRING_PTR(value) == (char*)NULL) return Qnil;

    if (RSTRING_PTR(value)[0] == '\0'
        || strcmp(RSTRING_PTR(value), "0") == 0
        || strcmp(RSTRING_PTR(value), "no") == 0
        || strcmp(RSTRING_PTR(value), "off") == 0
        || strcmp(RSTRING_PTR(value), "false") == 0) {
        return Qfalse;
    } else {
        return Qtrue;
    }
}

#if 0
static VALUE
tkstr_to_dec(value)
    VALUE value;
{
    return rb_cstr_to_inum(RSTRING_PTR(value), 10, 1);
}
#endif

static VALUE
tkstr_to_int(value)
    VALUE value;
{
    return rb_cstr_to_inum(RSTRING_PTR(value), 0, 1);
}

static VALUE
tkstr_to_float(value)
    VALUE value;
{
    return rb_float_new(rb_cstr_to_dbl(RSTRING_PTR(value), 1));
}

static VALUE
tkstr_invalid_numstr(value)
    VALUE value;
{
    rb_raise(rb_eArgError,
             "invalid value for Number: '%s'", RSTRING_PTR(value));
    return Qnil; /*dummy*/
}

static VALUE
tkstr_rescue_float(value)
    VALUE value;
{
    return rb_rescue2(tkstr_to_float, value,
                      tkstr_invalid_numstr, value,
                      rb_eArgError, 0);
}

static VALUE
tkstr_to_number(value)
    VALUE value;
{
    rb_check_type(value, T_STRING);

    if (RSTRING_PTR(value) == (char*)NULL) return INT2FIX(0);

    return rb_rescue2(tkstr_to_int, value,
                      tkstr_rescue_float, value,
                      rb_eArgError, 0);
}

static VALUE
tcl2rb_number(self, value)
    VALUE self;
    VALUE value;
{
    return tkstr_to_number(value);
}

static VALUE
tkstr_to_str(value)
    VALUE value;
{
    char * ptr;
    long len;

    ptr = RSTRING_PTR(value);
    len = RSTRING_LEN(value);

    if (len > 1 && *ptr == '{' && *(ptr + len - 1) == '}') {
        return rb_str_new(ptr + 1, len - 2);
    }
    return value;
}

static VALUE
tcl2rb_string(self, value)
    VALUE self;
    VALUE value;
{
    rb_check_type(value, T_STRING);

    if (RSTRING_PTR(value) == (char*)NULL) return rb_tainted_str_new2("");

    return tkstr_to_str(value);
}

static VALUE
tcl2rb_num_or_str(self, value)
    VALUE self;
    VALUE value;
{
    rb_check_type(value, T_STRING);

    if (RSTRING_PTR(value) == (char*)NULL) return rb_tainted_str_new2("");

    return rb_rescue2(tkstr_to_number, value,
                      tkstr_to_str, value,
                      rb_eArgError, 0);
}

static VALUE
tcl2rb_num_or_nil(self, value)
    VALUE self;
    VALUE value;
{
    rb_check_type(value, T_STRING);

    if (RSTRING_LEN(value) == 0) return Qnil;

    return tkstr_to_number(value);
}


/*************************************/

#define CBSUBST_TBL_MAX (256)
struct cbsubst_info {
    long  full_subst_length;
    long  keylen[CBSUBST_TBL_MAX];
    char  *key[CBSUBST_TBL_MAX];
    char  type[CBSUBST_TBL_MAX];
    ID    ivar[CBSUBST_TBL_MAX];
    VALUE proc;
    VALUE aliases;
};

static void
subst_mark(ptr)
    struct cbsubst_info *ptr;
{
    rb_gc_mark(ptr->proc);
    rb_gc_mark(ptr->aliases);
}

static void
subst_free(ptr)
    struct cbsubst_info *ptr;
{
    int i;

    if (ptr) {
      for(i = 0; i < CBSUBST_TBL_MAX; i++) {
	if (ptr->key[i] != NULL) {
	  free(ptr->key[i]); /* allocated by malloc */
	  ptr->key[i] = NULL;
	}
      }
      xfree(ptr); /* allocated by ALLOC */
    }
}

static VALUE
allocate_cbsubst_info(struct cbsubst_info **inf_ptr)
{
  struct cbsubst_info *inf;
  volatile VALUE proc, aliases;
  int idx;

  inf = ALLOC(struct cbsubst_info);

  inf->full_subst_length = 0;

  for(idx = 0; idx < CBSUBST_TBL_MAX; idx++) {
    inf->keylen[idx] = 0;
    inf->key[idx]    = NULL;
    inf->type[idx]   = '\0';
    inf->ivar[idx]   = (ID) 0;
  }

  proc = rb_hash_new();
  inf->proc = proc;

  aliases = rb_hash_new();
  inf->aliases = aliases;

  if (inf_ptr != (struct cbsubst_info **)NULL) *inf_ptr = inf;

  return Data_Wrap_Struct(cSUBST_INFO, subst_mark, subst_free, inf);
}

static void
cbsubst_init()
{
  rb_const_set(cCB_SUBST, ID_SUBST_INFO,
	       allocate_cbsubst_info((struct cbsubst_info **)NULL));
}

static VALUE
cbsubst_initialize(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    struct cbsubst_info *inf;
    int idx, iv_idx;

    Data_Get_Struct(rb_const_get(rb_obj_class(self), ID_SUBST_INFO),
                    struct cbsubst_info, inf);

   idx = 0;
    for(iv_idx = 0; iv_idx < CBSUBST_TBL_MAX; iv_idx++) {
      if ( inf->ivar[iv_idx] == (ID) 0 ) continue;
      rb_ivar_set(self, inf->ivar[iv_idx], argv[idx++]);
      if (idx >= argc) break;
    }

    return self;
}

static VALUE
cbsubst_ret_val(self, val)
    VALUE self;
    VALUE val;
{
    /* This method may be overwritten on some sub-classes.                  */
    /* This method is used for converting from ruby's callback-return-value */
    /* to tcl's value (e.g. validation procedure of entry widget).          */
    return val;
}

static int
each_attr_def(key, value, klass)
    VALUE key, value, klass;
{
    ID key_id, value_id;

    if (key == Qundef) return ST_CONTINUE;

    switch(TYPE(key)) {
    case T_STRING:
        key_id = rb_intern(RSTRING_PTR(key));
        break;
    case T_SYMBOL:
        key_id = SYM2ID(key);
        break;
    default:
        rb_raise(rb_eArgError,
                 "includes invalid key(s). expected a String or a Symbol");
    }

    switch(TYPE(value)) {
    case T_STRING:
        value_id = rb_intern(RSTRING_PTR(value));
        break;
    case T_SYMBOL:
        value_id = SYM2ID(value);
        break;
    default:
        rb_raise(rb_eArgError,
                 "includes invalid value(s). expected a String or a Symbol");
    }

    rb_alias(klass, key_id, value_id);

    return ST_CONTINUE;
}

static VALUE
cbsubst_def_attr_aliases(self, tbl)
    VALUE self;
    VALUE tbl;
{
    struct cbsubst_info *inf;

    if (TYPE(tbl) != T_HASH) {
        rb_raise(rb_eArgError, "expected a Hash");
    }

    Data_Get_Struct(rb_const_get(self, ID_SUBST_INFO),
                    struct cbsubst_info, inf);

    rb_hash_foreach(tbl, each_attr_def, self);

    return rb_funcall(inf->aliases, rb_intern("update"), 1, tbl);
}

static VALUE
cbsubst_sym_to_subst(self, sym)
    VALUE self;
    VALUE sym;
{
    struct cbsubst_info *inf;
    const char *str;
    char *buf, *ptr;
    int idx;
    long len;
    ID id;
    volatile VALUE ret;

    if (TYPE(sym) != T_SYMBOL) return sym;

    Data_Get_Struct(rb_const_get(self, ID_SUBST_INFO),
                    struct cbsubst_info, inf);

    if (!NIL_P(ret = rb_hash_aref(inf->aliases, sym))) {
      str = rb_id2name(SYM2ID(ret));
    } else {
      str = rb_id2name(SYM2ID(sym));
    }

    id = rb_intern(RSTRING_PTR(rb_str_cat2(rb_str_new2("@"), str)));

    for(idx = 0; idx < CBSUBST_TBL_MAX; idx++) {
      if (inf->ivar[idx] == id) break;
    }
    if (idx >= CBSUBST_TBL_MAX)  return sym;

    ptr = buf = ALLOC_N(char, inf->full_subst_length + 1);

    *(ptr++) = '%';

    if (len = inf->keylen[idx]) {
      /* longname */
      strncpy(ptr, inf->key[idx], len);
      ptr += len;
    } else {
      /* single char */
      *(ptr++) = (unsigned char)idx;
    }

    *(ptr++) = ' ';
    *(ptr++) = '\0';

    ret = rb_str_new2(buf);

    xfree(buf);

    return ret;
}

static VALUE
cbsubst_get_subst_arg(argc, argv, self)
    int   argc;
    VALUE *argv;
    VALUE self;
{
    struct cbsubst_info *inf;
    const char *str;
    char *buf, *ptr;
    int i, idx;
    long len;
    ID id;
    volatile VALUE arg_sym, ret;

    Data_Get_Struct(rb_const_get(self, ID_SUBST_INFO),
                    struct cbsubst_info, inf);

    ptr = buf = ALLOC_N(char, inf->full_subst_length + 1);

    for(i = 0; i < argc; i++) {
        switch(TYPE(argv[i])) {
        case T_STRING:
            str = RSTRING_PTR(argv[i]);
            arg_sym = ID2SYM(rb_intern(str));
            break;
        case T_SYMBOL:
            arg_sym = argv[i];
            str = rb_id2name(SYM2ID(arg_sym));
            break;
        default:
            rb_raise(rb_eArgError, "arg #%d is not a String or a Symbol", i);
        }

        if (!NIL_P(ret = rb_hash_aref(inf->aliases, arg_sym))) {
            str = rb_id2name(SYM2ID(ret));
        }

        id = rb_intern(RSTRING_PTR(rb_str_cat2(rb_str_new2("@"), str)));

	for(idx = 0; idx < CBSUBST_TBL_MAX; idx++) {
	  if (inf->ivar[idx] == id) break;
	}
        if (idx >= CBSUBST_TBL_MAX) {
            rb_raise(rb_eArgError, "cannot find attribute :%s", str);
        }

	*(ptr++) = '%';

	if (len = inf->keylen[idx]) {
	  /* longname */
	  strncpy(ptr, inf->key[idx], len);
	  ptr += len;
	} else {
	  /* single char */
	  *(ptr++) = (unsigned char)idx;
	}

	*(ptr++) = ' ';
    }

    *ptr = '\0';

    ret = rb_str_new2(buf);

    xfree(buf);

    return ret;
}

static VALUE
cbsubst_get_subst_key(self, str)
    VALUE self;
    VALUE str;
{
    struct cbsubst_info *inf;
    volatile VALUE list;
    volatile VALUE ret;
    VALUE keyval;
    long i, len, keylen;
    int idx;
    char *buf, *ptr, *key;

    list = rb_funcall(cTclTkLib, ID_split_tklist, 1, str);
    len = RARRAY_LEN(list);

    Data_Get_Struct(rb_const_get(self, ID_SUBST_INFO),
                    struct cbsubst_info, inf);

    ptr = buf = ALLOC_N(char, inf->full_subst_length + len + 1);

    for(i = 0; i < len; i++) {
      keyval = RARRAY_PTR(list)[i];
      key = RSTRING_PTR(keyval);
      if (*key == '%') {
	if (*(key + 2) == '\0') {
	  /* single char */
	  *(ptr++) = *(key + 1);
	} else {
	  /* search longname-key */
	  keylen = RSTRING_LEN(keyval) - 1;
	  for(idx = 0; idx < CBSUBST_TBL_MAX; idx++) {
	    if (inf->keylen[idx] != keylen) continue;
	    if ((unsigned char)inf->key[idx][0] != (unsigned char)*(key + 1)) continue;
	    if (strncmp(inf->key[idx], key + 1, keylen)) continue;
	    break;
	  }
	  if (idx < CBSUBST_TBL_MAX) {
	    *(ptr++) = (unsigned char)idx;
	  } else {
	    *(ptr++) = ' ';
	  }
	}
      } else {
	*(ptr++) = ' ';
      }
    }
    *ptr = '\0';

    ret = rb_str_new2(buf);
    xfree(buf);
    return ret;
}

static VALUE
cbsubst_get_all_subst_keys(self)
    VALUE self;
{
    struct cbsubst_info *inf;
    char *buf, *ptr;
    char *keys_buf, *keys_ptr;
    int idx;
    long len;
    volatile VALUE ret;

    Data_Get_Struct(rb_const_get(self, ID_SUBST_INFO),
                    struct cbsubst_info, inf);

    ptr = buf = ALLOC_N(char, inf->full_subst_length + 1);
    keys_ptr = keys_buf = ALLOC_N(char, CBSUBST_TBL_MAX + 1);

    for(idx = 0; idx < CBSUBST_TBL_MAX; idx++) {
      if (inf->ivar[idx] == (ID) 0) continue;

      *(keys_ptr++) = (unsigned char)idx;

      *(ptr++) = '%';

      if (len = inf->keylen[idx]) {
	/* longname */
	strncpy(ptr, inf->key[idx], len);
	ptr += len;
      } else {
	/* single char */
	*(ptr++) = (unsigned char)idx;
      }

      *(ptr++) = ' ';
    }

    *ptr = '\0';
    *keys_ptr = '\0';

    ret = rb_ary_new3(2, rb_str_new2(keys_buf), rb_str_new2(buf));

    xfree(buf);
    xfree(keys_buf);

    return ret;
}

static VALUE
cbsubst_table_setup(argc, argv, self)
     int   argc;
     VALUE *argv;
     VALUE self;
{
  volatile VALUE cbsubst_obj;
  volatile VALUE key_inf;
  volatile VALUE longkey_inf;
  volatile VALUE proc_inf;
  VALUE inf;
  ID id;
  struct cbsubst_info *subst_inf;
  long idx, len;
  unsigned char chr;

  /* accept (key_inf, proc_inf) or (key_inf, longkey_inf, procinf) */
  if (rb_scan_args(argc, argv, "21", &key_inf, &longkey_inf, &proc_inf) == 2) {
    proc_inf = longkey_inf;
    longkey_inf = rb_ary_new();
  }

  /* check the number of longkeys */
  if (RARRAY_LEN(longkey_inf) > 125 /* from 0x80 to 0xFD */) {
    rb_raise(rb_eArgError, "too many longname-key definitions");
  }

  /* init */
  cbsubst_obj = allocate_cbsubst_info(&subst_inf);

  /*
   * keys : array of [subst, type, ivar]
   *         subst ==> char code or string
   *         type  ==> char code or string
   *         ivar  ==> symbol
   */
  len = RARRAY_LEN(key_inf);
  for(idx = 0; idx < len; idx++) {
    inf = RARRAY_PTR(key_inf)[idx];
    if (TYPE(inf) != T_ARRAY) continue;

    if (TYPE(RARRAY_PTR(inf)[0]) == T_STRING) {
      chr = *(RSTRING_PTR(RARRAY_PTR(inf)[0]));
    } else {
      chr = NUM2CHR(RARRAY_PTR(inf)[0]);
    }
    if (TYPE(RARRAY_PTR(inf)[1]) == T_STRING) {
      subst_inf->type[chr] = *(RSTRING_PTR(RARRAY_PTR(inf)[1]));
    } else {
      subst_inf->type[chr] = NUM2CHR(RARRAY_PTR(inf)[1]);
    }

    subst_inf->full_subst_length += 3;

    id = SYM2ID(RARRAY_PTR(inf)[2]);
    subst_inf->ivar[chr] = rb_intern(RSTRING_PTR(rb_str_cat2(rb_str_new2("@"), rb_id2name(id))));

    rb_attr(self, id, 1, 0, Qtrue);
  }


  /*
   * longkeys : array of [name, type, ivar]
   *         name ==> longname key string
   *         type ==> char code or string
   *         ivar ==> symbol
   */
  len = RARRAY_LEN(longkey_inf);
  for(idx = 0; idx < len; idx++) {
    inf = RARRAY_PTR(longkey_inf)[idx];
    if (TYPE(inf) != T_ARRAY) continue;

    chr = (unsigned char)(0x80 + idx);
    subst_inf->keylen[chr] = RSTRING_LEN(RARRAY_PTR(inf)[0]);
#if HAVE_STRNDUP
    subst_inf->key[chr] = strndup(RSTRING_PTR(RARRAY_PTR(inf)[0]),
				  RSTRING_LEN(RARRAY_PTR(inf)[0]));
#else
    subst_inf->key[chr] = malloc(RSTRING_LEN(RARRAY_PTR(inf)[0]) + 1);
    if (subst_inf->key[chr]) {
      strncpy(subst_inf->key[chr], RSTRING_PTR(RARRAY_PTR(inf)[0]),
	      RSTRING_LEN(RARRAY_PTR(inf)[0]) + 1);
      subst_inf->key[chr][RSTRING_LEN(RARRAY_PTR(inf)[0])] = '\0';
    }
#endif
    if (TYPE(RARRAY_PTR(inf)[1]) == T_STRING) {
      subst_inf->type[chr] = *(RSTRING_PTR(RARRAY_PTR(inf)[1]));
    } else {
      subst_inf->type[chr] = NUM2CHR(RARRAY_PTR(inf)[1]);
    }

    subst_inf->full_subst_length += (subst_inf->keylen[chr] + 2);

    id = SYM2ID(RARRAY_PTR(inf)[2]);
    subst_inf->ivar[chr] = rb_intern(RSTRING_PTR(rb_str_cat2(rb_str_new2("@"), rb_id2name(id))));

    rb_attr(self, id, 1, 0, Qtrue);
  }

  /*
   * procs : array of [type, proc]
   *         type  ==> char code or string
   *         proc  ==> proc/method/obj (must respond to 'call')
   */
  len = RARRAY_LEN(proc_inf);
  for(idx = 0; idx < len; idx++) {
    inf = RARRAY_PTR(proc_inf)[idx];
    if (TYPE(inf) != T_ARRAY) continue;
    rb_hash_aset(subst_inf->proc,
		 ((TYPE(RARRAY_PTR(inf)[0]) == T_STRING)?
		  INT2FIX(*(RSTRING_PTR(RARRAY_PTR(inf)[0]))) :
		  RARRAY_PTR(inf)[0]),
		 RARRAY_PTR(inf)[1]);
  }

  rb_const_set(self, ID_SUBST_INFO, cbsubst_obj);

  return self;
}

static VALUE
cbsubst_get_extra_args_tbl(self)
    VALUE self;
{
  return rb_ary_new();
}

static VALUE
cbsubst_scan_args(self, arg_key, val_ary)
    VALUE self;
    VALUE arg_key;
    VALUE val_ary;
{
    struct cbsubst_info *inf;
    long idx;
    unsigned char *keyptr = (unsigned char*)RSTRING_PTR(arg_key);
    long keylen = RSTRING_LEN(arg_key);
    long vallen = RARRAY_LEN(val_ary);
    unsigned char type_chr;
    volatile VALUE dst = rb_ary_new2(vallen);
    volatile VALUE proc;
    int thr_crit_bup;
    VALUE old_gc;

    thr_crit_bup = rb_thread_critical;
    rb_thread_critical = Qtrue;

    old_gc = rb_gc_disable();

    Data_Get_Struct(rb_const_get(self, ID_SUBST_INFO),
                    struct cbsubst_info, inf);

    for(idx = 0; idx < vallen; idx++) {
      if (idx >= keylen) {
	proc = Qnil;
      } else if (*(keyptr + idx) == ' ') {
	proc = Qnil;
      } else {
	if (type_chr = inf->type[*(keyptr + idx)]) {
	  proc = rb_hash_aref(inf->proc, INT2FIX((int)type_chr));
	} else {
	  proc = Qnil;
	}
      }

      if (NIL_P(proc)) {
	rb_ary_push(dst, RARRAY_PTR(val_ary)[idx]);
      } else {
	rb_ary_push(dst, rb_funcall(proc, ID_call, 1,
				    RARRAY_PTR(val_ary)[idx]));
      }
    }

    if (old_gc == Qfalse) rb_gc_enable();
    rb_thread_critical = thr_crit_bup;

    return dst;
}

static VALUE
cbsubst_inspect(self)
    VALUE self;
{
    return rb_str_new2("CallbackSubst");
}

static VALUE
substinfo_inspect(self)
    VALUE self;
{
    return rb_str_new2("SubstInfo");
}

/*************************************/

static VALUE
tk_cbe_inspect(self)
    VALUE self;
{
    return rb_str_new2("TkCallbackEntry");
}

/*************************************/

static VALUE
tkobj_path(self)
    VALUE self;
{
    return rb_ivar_get(self, ID_at_path);
}


/*************************************/
/* release date */
const char tkutil_release_date[] = TKUTIL_RELEASE_DATE;

void
Init_tkutil()
{
    VALUE cTK = rb_define_class("TkKernel", rb_cObject);
    VALUE mTK = rb_define_module("TkUtil");

    /* --------------------- */

    rb_define_const(mTK, "RELEASE_DATE",
                    rb_obj_freeze(rb_str_new2(tkutil_release_date)));

    /* --------------------- */
    rb_global_variable(&cMethod);
    cMethod = rb_const_get(rb_cObject, rb_intern("Method"));

    ID_path = rb_intern("path");
    ID_at_path = rb_intern("@path");
    ID_at_enc = rb_intern("@encoding");
    ID_to_eval = rb_intern("to_eval");
    ID_to_s = rb_intern("to_s");
    ID_source = rb_intern("source");
    ID_downcase = rb_intern("downcase");
    ID_install_cmd = rb_intern("install_cmd");
    ID_merge_tklist = rb_intern("_merge_tklist");
    ID_encoding = rb_intern("encoding");
    ID_encoding_system = rb_intern("encoding_system");
    ID_call = rb_intern("call");

    /* --------------------- */
    cCB_SUBST = rb_define_class_under(mTK, "CallbackSubst", rb_cObject);
    rb_define_singleton_method(cCB_SUBST, "inspect", cbsubst_inspect, 0);

    cSUBST_INFO = rb_define_class_under(cCB_SUBST, "Info", rb_cObject);
    rb_define_singleton_method(cSUBST_INFO, "inspect", substinfo_inspect, 0);

    ID_SUBST_INFO = rb_intern("SUBST_INFO");
    rb_define_singleton_method(cCB_SUBST, "ret_val", cbsubst_ret_val, 1);
    rb_define_singleton_method(cCB_SUBST, "scan_args", cbsubst_scan_args, 2);
    rb_define_singleton_method(cCB_SUBST, "_sym2subst",
			       cbsubst_sym_to_subst, 1);
    rb_define_singleton_method(cCB_SUBST, "subst_arg",
                               cbsubst_get_subst_arg, -1);
    rb_define_singleton_method(cCB_SUBST, "_get_subst_key",
                               cbsubst_get_subst_key,  1);
    rb_define_singleton_method(cCB_SUBST, "_get_all_subst_keys",
                               cbsubst_get_all_subst_keys,  0);
    rb_define_singleton_method(cCB_SUBST, "_setup_subst_table",
                               cbsubst_table_setup, -1);
    rb_define_singleton_method(cCB_SUBST, "_get_extra_args_tbl",
                               cbsubst_get_extra_args_tbl,  0);
    rb_define_singleton_method(cCB_SUBST, "_define_attribute_aliases",
                               cbsubst_def_attr_aliases,  1);

    rb_define_method(cCB_SUBST, "initialize", cbsubst_initialize, -1);

    cbsubst_init();

    /* --------------------- */
    rb_global_variable(&cTkCallbackEntry);
    cTkCallbackEntry = rb_define_class("TkCallbackEntry", cTK);
    rb_define_singleton_method(cTkCallbackEntry, "inspect", tk_cbe_inspect, 0);

    /* --------------------- */
    rb_global_variable(&cTkObject);
    cTkObject = rb_define_class("TkObject", cTK);
    rb_define_method(cTkObject, "path", tkobj_path, 0);

    /* --------------------- */
    rb_require("tcltklib");
    rb_global_variable(&cTclTkLib);
    cTclTkLib = rb_const_get(rb_cObject, rb_intern("TclTkLib"));
    ID_split_tklist = rb_intern("_split_tklist");
    ID_toUTF8 = rb_intern("_toUTF8");
    ID_fromUTF8 = rb_intern("_fromUTF8");

    /* --------------------- */
    rb_define_singleton_method(cTK, "new", tk_s_new, -1);

    /* --------------------- */
    rb_global_variable(&TK_None);
    TK_None = rb_obj_alloc(rb_cObject);
    rb_define_const(mTK, "None", TK_None);
    rb_define_singleton_method(TK_None, "to_s", tkNone_to_s, 0);
    rb_define_singleton_method(TK_None, "inspect", tkNone_inspect, 0);
    OBJ_FREEZE(TK_None);

    /* --------------------- */
    rb_global_variable(&CALLBACK_TABLE);
    CALLBACK_TABLE = rb_hash_new();

    /* --------------------- */
    rb_define_singleton_method(mTK, "untrust", tk_obj_untrust, 1);

    rb_define_singleton_method(mTK, "eval_cmd", tk_eval_cmd, -1);
    rb_define_singleton_method(mTK, "callback", tk_do_callback, -1);
    rb_define_singleton_method(mTK, "install_cmd", tk_install_cmd, -1);
    rb_define_singleton_method(mTK, "uninstall_cmd", tk_uninstall_cmd, 1);
    rb_define_singleton_method(mTK, "_symbolkey2str", tk_symbolkey2str, 1);
    rb_define_singleton_method(mTK, "hash_kv", tk_hash_kv, -1);
    rb_define_singleton_method(mTK, "_get_eval_string",
                               tk_get_eval_string, -1);
    rb_define_singleton_method(mTK, "_get_eval_enc_str",
                               tk_get_eval_enc_str, 1);
    rb_define_singleton_method(mTK, "_conv_args", tk_conv_args, -1);

    rb_define_singleton_method(mTK, "bool", tcl2rb_bool, 1);
    rb_define_singleton_method(mTK, "number", tcl2rb_number, 1);
    rb_define_singleton_method(mTK, "string", tcl2rb_string, 1);
    rb_define_singleton_method(mTK, "num_or_str", tcl2rb_num_or_str, 1);
    rb_define_singleton_method(mTK, "num_or_nil", tcl2rb_num_or_nil, 1);

    rb_define_method(mTK, "_toUTF8", tk_toUTF8, -1);
    rb_define_method(mTK, "_fromUTF8", tk_fromUTF8, -1);
    rb_define_method(mTK, "_symbolkey2str", tk_symbolkey2str, 1);
    rb_define_method(mTK, "hash_kv", tk_hash_kv, -1);
    rb_define_method(mTK, "_get_eval_string", tk_get_eval_string, -1);
    rb_define_method(mTK, "_get_eval_enc_str", tk_get_eval_enc_str, 1);
    rb_define_method(mTK, "_conv_args", tk_conv_args, -1);

    rb_define_method(mTK, "bool", tcl2rb_bool, 1);
    rb_define_method(mTK, "number", tcl2rb_number, 1);
    rb_define_method(mTK, "string", tcl2rb_string, 1);
    rb_define_method(mTK, "num_or_str", tcl2rb_num_or_str, 1);
    rb_define_method(mTK, "num_or_nil", tcl2rb_num_or_nil, 1);

    /* --------------------- */
    rb_global_variable(&ENCODING_NAME_UTF8);
    ENCODING_NAME_UTF8 = rb_obj_freeze(rb_str_new2("utf-8"));

    /* --------------------- */
}
