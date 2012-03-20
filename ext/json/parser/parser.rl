#include "../fbuffer/fbuffer.h"
#include "parser.h"

/* unicode */

static const char digit_values[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1,
    -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1
};

static UTF32 unescape_unicode(const unsigned char *p)
{
    char b;
    UTF32 result = 0;
    b = digit_values[p[0]];
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    result = (result << 4) | b;
    b = digit_values[p[1]];
    result = (result << 4) | b;
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    b = digit_values[p[2]];
    result = (result << 4) | b;
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    b = digit_values[p[3]];
    result = (result << 4) | b;
    if (b < 0) return UNI_REPLACEMENT_CHAR;
    return result;
}

static int convert_UTF32_to_UTF8(char *buf, UTF32 ch)
{
    int len = 1;
    if (ch <= 0x7F) {
        buf[0] = (char) ch;
    } else if (ch <= 0x07FF) {
        buf[0] = (char) ((ch >> 6) | 0xC0);
        buf[1] = (char) ((ch & 0x3F) | 0x80);
        len++;
    } else if (ch <= 0xFFFF) {
        buf[0] = (char) ((ch >> 12) | 0xE0);
        buf[1] = (char) (((ch >> 6) & 0x3F) | 0x80);
        buf[2] = (char) ((ch & 0x3F) | 0x80);
        len += 2;
    } else if (ch <= 0x1fffff) {
        buf[0] =(char) ((ch >> 18) | 0xF0);
        buf[1] =(char) (((ch >> 12) & 0x3F) | 0x80);
        buf[2] =(char) (((ch >> 6) & 0x3F) | 0x80);
        buf[3] =(char) ((ch & 0x3F) | 0x80);
        len += 3;
    } else {
        buf[0] = '?';
    }
    return len;
}

#ifdef HAVE_RUBY_ENCODING_H
static VALUE CEncoding_ASCII_8BIT, CEncoding_UTF_8, CEncoding_UTF_16BE,
    CEncoding_UTF_16LE, CEncoding_UTF_32BE, CEncoding_UTF_32LE;
static ID i_encoding, i_encode;
#else
static ID i_iconv;
#endif

static VALUE mJSON, mExt, cParser, eParserError, eNestingError;
static VALUE CNaN, CInfinity, CMinusInfinity;

static ID i_json_creatable_p, i_json_create, i_create_id, i_create_additions,
          i_chr, i_max_nesting, i_allow_nan, i_symbolize_names, i_quirks_mode,
          i_object_class, i_array_class, i_key_p, i_deep_const_get, i_match,
          i_match_string, i_aset, i_aref, i_leftshift;

%%{
    machine JSON_common;

    cr                  = '\n';
    cr_neg              = [^\n];
    ws                  = [ \t\r\n];
    c_comment           = '/*' ( any* - (any* '*/' any* ) ) '*/';
    cpp_comment         = '//' cr_neg* cr;
    comment             = c_comment | cpp_comment;
    ignore              = ws | comment;
    name_separator      = ':';
    value_separator     = ',';
    Vnull               = 'null';
    Vfalse              = 'false';
    Vtrue               = 'true';
    VNaN                = 'NaN';
    VInfinity           = 'Infinity';
    VMinusInfinity      = '-Infinity';
    begin_value         = [nft\"\-\[\{NI] | digit;
    begin_object        = '{';
    end_object          = '}';
    begin_array         = '[';
    end_array           = ']';
    begin_string        = '"';
    begin_name          = begin_string;
    begin_number        = digit | '-';
}%%

%%{
    machine JSON_object;
    include JSON_common;

    write data;

    action parse_value {
        VALUE v = Qnil;
        char *np = JSON_parse_value(json, fpc, pe, &v);
        if (np == NULL) {
            fhold; fbreak;
        } else {
            if (NIL_P(json->object_class)) {
                rb_hash_aset(*result, last_name, v);
            } else {
                rb_funcall(*result, i_aset, 2, last_name, v);
            }
            fexec np;
        }
    }

    action parse_name {
        char *np;
        json->parsing_name = 1;
        np = JSON_parse_string(json, fpc, pe, &last_name);
        json->parsing_name = 0;
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action exit { fhold; fbreak; }

    pair  = ignore* begin_name >parse_name ignore* name_separator ignore* begin_value >parse_value;
    next_pair   = ignore* value_separator pair;

    main := (
      begin_object
      (pair (next_pair)*)? ignore*
      end_object
    ) @exit;
}%%

static char *JSON_parse_object(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;
    VALUE last_name = Qnil;
    VALUE object_class = json->object_class;

    if (json->max_nesting && json->current_nesting > json->max_nesting) {
        rb_raise(eNestingError, "nesting of %d is too deep", json->current_nesting);
    }

    *result = NIL_P(object_class) ? rb_hash_new() : rb_class_new_instance(0, 0, object_class);

    %% write init;
    %% write exec;

    if (cs >= JSON_object_first_final) {
        if (json->create_additions) {
            VALUE klassname;
            if (NIL_P(json->object_class)) {
              klassname = rb_hash_aref(*result, json->create_id);
            } else {
              klassname = rb_funcall(*result, i_aref, 1, json->create_id);
            }
            if (!NIL_P(klassname)) {
                VALUE klass = rb_funcall(mJSON, i_deep_const_get, 1, klassname);
                if (RTEST(rb_funcall(klass, i_json_creatable_p, 0))) {
                    *result = rb_funcall(klass, i_json_create, 1, *result);
                }
            }
        }
        return p + 1;
    } else {
        return NULL;
    }
}


%%{
    machine JSON_value;
    include JSON_common;

    write data;

    action parse_null {
        *result = Qnil;
    }
    action parse_false {
        *result = Qfalse;
    }
    action parse_true {
        *result = Qtrue;
    }
    action parse_nan {
        if (json->allow_nan) {
            *result = CNaN;
        } else {
            rb_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p - 2);
        }
    }
    action parse_infinity {
        if (json->allow_nan) {
            *result = CInfinity;
        } else {
            rb_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p - 8);
        }
    }
    action parse_string {
        char *np = JSON_parse_string(json, fpc, pe, result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_number {
        char *np;
        if(pe > fpc + 9 - json->quirks_mode && !strncmp(MinusInfinity, fpc, 9)) {
            if (json->allow_nan) {
                *result = CMinusInfinity;
                fexec p + 10;
                fhold; fbreak;
            } else {
                rb_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
            }
        }
        np = JSON_parse_float(json, fpc, pe, result);
        if (np != NULL) fexec np;
        np = JSON_parse_integer(json, fpc, pe, result);
        if (np != NULL) fexec np;
        fhold; fbreak;
    }

    action parse_array {
        char *np;
        json->current_nesting++;
        np = JSON_parse_array(json, fpc, pe, result);
        json->current_nesting--;
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_object {
        char *np;
        json->current_nesting++;
        np =  JSON_parse_object(json, fpc, pe, result);
        json->current_nesting--;
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action exit { fhold; fbreak; }

main := (
              Vnull @parse_null |
              Vfalse @parse_false |
              Vtrue @parse_true |
              VNaN @parse_nan |
              VInfinity @parse_infinity |
              begin_number >parse_number |
              begin_string >parse_string |
              begin_array >parse_array |
              begin_object >parse_object
        ) %*exit;
}%%

static char *JSON_parse_value(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;

    %% write init;
    %% write exec;

    if (cs >= JSON_value_first_final) {
        return p;
    } else {
        return NULL;
    }
}

%%{
    machine JSON_integer;

    write data;

    action exit { fhold; fbreak; }

    main := '-'? ('0' | [1-9][0-9]*) (^[0-9]? @exit);
}%%

static char *JSON_parse_integer(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;

    %% write init;
    json->memo = p;
    %% write exec;

    if (cs >= JSON_integer_first_final) {
        long len = p - json->memo;
        fbuffer_clear(json->fbuffer);
        fbuffer_append(json->fbuffer, json->memo, len);
        fbuffer_append_char(json->fbuffer, '\0');
        *result = rb_cstr2inum(FBUFFER_PTR(json->fbuffer), 10);
        return p + 1;
    } else {
        return NULL;
    }
}

%%{
    machine JSON_float;
    include JSON_common;

    write data;

    action exit { fhold; fbreak; }

    main := '-'? (
              (('0' | [1-9][0-9]*) '.' [0-9]+ ([Ee] [+\-]?[0-9]+)?)
              | (('0' | [1-9][0-9]*) ([Ee] [+\-]?[0-9]+))
             )  (^[0-9Ee.\-]? @exit );
}%%

static char *JSON_parse_float(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;

    %% write init;
    json->memo = p;
    %% write exec;

    if (cs >= JSON_float_first_final) {
        long len = p - json->memo;
        fbuffer_clear(json->fbuffer);
        fbuffer_append(json->fbuffer, json->memo, len);
        fbuffer_append_char(json->fbuffer, '\0');
        *result = rb_float_new(rb_cstr_to_dbl(FBUFFER_PTR(json->fbuffer), 1));
        return p + 1;
    } else {
        return NULL;
    }
}


%%{
    machine JSON_array;
    include JSON_common;

    write data;

    action parse_value {
        VALUE v = Qnil;
        char *np = JSON_parse_value(json, fpc, pe, &v);
        if (np == NULL) {
            fhold; fbreak;
        } else {
            if (NIL_P(json->array_class)) {
                rb_ary_push(*result, v);
            } else {
                rb_funcall(*result, i_leftshift, 1, v);
            }
            fexec np;
        }
    }

    action exit { fhold; fbreak; }

    next_element  = value_separator ignore* begin_value >parse_value;

    main := begin_array ignore*
          ((begin_value >parse_value ignore*)
           (ignore* next_element ignore*)*)?
          end_array @exit;
}%%

static char *JSON_parse_array(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;
    VALUE array_class = json->array_class;

    if (json->max_nesting && json->current_nesting > json->max_nesting) {
        rb_raise(eNestingError, "nesting of %d is too deep", json->current_nesting);
    }
    *result = NIL_P(array_class) ? rb_ary_new() : rb_class_new_instance(0, 0, array_class);

    %% write init;
    %% write exec;

    if(cs >= JSON_array_first_final) {
        return p + 1;
    } else {
        rb_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return NULL;
    }
}

static VALUE json_string_unescape(VALUE result, char *string, char *stringEnd)
{
    char *p = string, *pe = string, *unescape;
    int unescape_len;
    char buf[4];

    while (pe < stringEnd) {
        if (*pe == '\\') {
            unescape = (char *) "?";
            unescape_len = 1;
            if (pe > p) rb_str_buf_cat(result, p, pe - p);
            switch (*++pe) {
                case 'n':
                    unescape = (char *) "\n";
                    break;
                case 'r':
                    unescape = (char *) "\r";
                    break;
                case 't':
                    unescape = (char *) "\t";
                    break;
                case '"':
                    unescape = (char *) "\"";
                    break;
                case '\\':
                    unescape = (char *) "\\";
                    break;
                case 'b':
                    unescape = (char *) "\b";
                    break;
                case 'f':
                    unescape = (char *) "\f";
                    break;
                case 'u':
                    if (pe > stringEnd - 4) {
                        return Qnil;
                    } else {
                        UTF32 ch = unescape_unicode((unsigned char *) ++pe);
                        pe += 3;
                        if (UNI_SUR_HIGH_START == (ch & 0xFC00)) {
                            pe++;
                            if (pe > stringEnd - 6) return Qnil;
                            if (pe[0] == '\\' && pe[1] == 'u') {
                                UTF32 sur = unescape_unicode((unsigned char *) pe + 2);
                                ch = (((ch & 0x3F) << 10) | ((((ch >> 6) & 0xF) + 1) << 16)
                                        | (sur & 0x3FF));
                                pe += 5;
                            } else {
                                unescape = (char *) "?";
                                break;
                            }
                        }
                        unescape_len = convert_UTF32_to_UTF8(buf, ch);
                        unescape = buf;
                    }
                    break;
                default:
                    p = pe;
                    continue;
            }
            rb_str_buf_cat(result, unescape, unescape_len);
            p = ++pe;
        } else {
            pe++;
        }
    }
    rb_str_buf_cat(result, p, pe - p);
    return result;
}

%%{
    machine JSON_string;
    include JSON_common;

    write data;

    action parse_string {
        *result = json_string_unescape(*result, json->memo + 1, p);
        if (NIL_P(*result)) {
            fhold;
            fbreak;
        } else {
            FORCE_UTF8(*result);
            fexec p + 1;
        }
    }

    action exit { fhold; fbreak; }

    main := '"' ((^([\"\\] | 0..0x1f) | '\\'[\"\\/bfnrt] | '\\u'[0-9a-fA-F]{4} | '\\'^([\"\\/bfnrtu]|0..0x1f))* %parse_string) '"' @exit;
}%%

static int
match_i(VALUE regexp, VALUE klass, VALUE memo)
{
    if (regexp == Qundef) return ST_STOP;
    if (RTEST(rb_funcall(klass, i_json_creatable_p, 0)) &&
      RTEST(rb_funcall(regexp, i_match, 1, rb_ary_entry(memo, 0)))) {
        rb_ary_push(memo, klass);
        return ST_STOP;
    }
    return ST_CONTINUE;
}

static char *JSON_parse_string(JSON_Parser *json, char *p, char *pe, VALUE *result)
{
    int cs = EVIL;
    VALUE match_string;

    *result = rb_str_buf_new(0);
    %% write init;
    json->memo = p;
    %% write exec;

    if (json->create_additions && RTEST(match_string = json->match_string)) {
          VALUE klass;
          VALUE memo = rb_ary_new2(2);
          rb_ary_push(memo, *result);
          rb_hash_foreach(match_string, match_i, memo);
          klass = rb_ary_entry(memo, 1);
          if (RTEST(klass)) {
              *result = rb_funcall(klass, i_json_create, 1, *result);
          }
    }

    if (json->symbolize_names && json->parsing_name) {
      *result = rb_str_intern(*result);
    }
    if (cs >= JSON_string_first_final) {
        return p + 1;
    } else {
        return NULL;
    }
}

/*
 * Document-class: JSON::Ext::Parser
 *
 * This is the JSON parser implemented as a C extension. It can be configured
 * to be used by setting
 *
 *  JSON.parser = JSON::Ext::Parser
 *
 * with the method parser= in JSON.
 *
 */

static VALUE convert_encoding(VALUE source)
{
    char *ptr = RSTRING_PTR(source);
    long len = RSTRING_LEN(source);
    if (len < 2) {
        rb_raise(eParserError, "A JSON text must at least contain two octets!");
    }
#ifdef HAVE_RUBY_ENCODING_H
    {
        VALUE encoding = rb_funcall(source, i_encoding, 0);
        if (encoding == CEncoding_ASCII_8BIT) {
            if (len >= 4 &&  ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0) {
                source = rb_funcall(source, i_encode, 2, CEncoding_UTF_8, CEncoding_UTF_32BE);
            } else if (len >= 4 && ptr[0] == 0 && ptr[2] == 0) {
                source = rb_funcall(source, i_encode, 2, CEncoding_UTF_8, CEncoding_UTF_16BE);
            } else if (len >= 4 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0) {
                source = rb_funcall(source, i_encode, 2, CEncoding_UTF_8, CEncoding_UTF_32LE);
            } else if (len >= 4 && ptr[1] == 0 && ptr[3] == 0) {
                source = rb_funcall(source, i_encode, 2, CEncoding_UTF_8, CEncoding_UTF_16LE);
            } else {
                source = rb_str_dup(source);
                FORCE_UTF8(source);
            }
        } else {
            source = rb_funcall(source, i_encode, 1, CEncoding_UTF_8);
        }
    }
#else
    if (len >= 4 &&  ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-32be"), source);
    } else if (len >= 4 && ptr[0] == 0 && ptr[2] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-16be"), source);
    } else if (len >= 4 && ptr[1] == 0 && ptr[2] == 0 && ptr[3] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-32le"), source);
    } else if (len >= 4 && ptr[1] == 0 && ptr[3] == 0) {
      source = rb_funcall(mJSON, i_iconv, 3, rb_str_new2("utf-8"), rb_str_new2("utf-16le"), source);
    }
#endif
    return source;
}

/*
 * call-seq: new(source, opts => {})
 *
 * Creates a new JSON::Ext::Parser instance for the string _source_.
 *
 * Creates a new JSON::Ext::Parser instance for the string _source_.
 *
 * It will be configured by the _opts_ hash. _opts_ can have the following
 * keys:
 *
 * _opts_ can have the following keys:
 * * *max_nesting*: The maximum depth of nesting allowed in the parsed data
 *   structures. Disable depth checking with :max_nesting => false|nil|0, it
 *   defaults to 19.
 * * *allow_nan*: If set to true, allow NaN, Infinity and -Infinity in
 *   defiance of RFC 4627 to be parsed by the Parser. This option defaults to
 *   false.
 * * *symbolize_names*: If set to true, returns symbols for the names
 *   (keys) in a JSON object. Otherwise strings are returned, which is also
 *   the default.
 * * *create_additions*: If set to false, the Parser doesn't create
 *   additions even if a matchin class and create_id was found. This option
 *   defaults to true.
 * * *object_class*: Defaults to Hash
 * * *array_class*: Defaults to Array
 */
static VALUE cParser_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE source, opts;
    GET_PARSER_INIT;

    if (json->Vsource) {
        rb_raise(rb_eTypeError, "already initialized instance");
    }
    rb_scan_args(argc, argv, "11", &source, &opts);
    if (!NIL_P(opts)) {
        opts = rb_convert_type(opts, T_HASH, "Hash", "to_hash");
        if (NIL_P(opts)) {
            rb_raise(rb_eArgError, "opts needs to be like a hash");
        } else {
            VALUE tmp = ID2SYM(i_max_nesting);
            if (option_given_p(opts, tmp)) {
                VALUE max_nesting = rb_hash_aref(opts, tmp);
                if (RTEST(max_nesting)) {
                    Check_Type(max_nesting, T_FIXNUM);
                    json->max_nesting = FIX2INT(max_nesting);
                } else {
                    json->max_nesting = 0;
                }
            } else {
                json->max_nesting = 19;
            }
            tmp = ID2SYM(i_allow_nan);
            if (option_given_p(opts, tmp)) {
                json->allow_nan = RTEST(rb_hash_aref(opts, tmp)) ? 1 : 0;
            } else {
                json->allow_nan = 0;
            }
            tmp = ID2SYM(i_symbolize_names);
            if (option_given_p(opts, tmp)) {
                json->symbolize_names = RTEST(rb_hash_aref(opts, tmp)) ? 1 : 0;
            } else {
                json->symbolize_names = 0;
            }
            tmp = ID2SYM(i_quirks_mode);
            if (option_given_p(opts, tmp)) {
                VALUE quirks_mode = rb_hash_aref(opts, tmp);
                json->quirks_mode = RTEST(quirks_mode) ? 1 : 0;
            } else {
                json->quirks_mode = 0;
            }
            tmp = ID2SYM(i_create_additions);
            if (option_given_p(opts, tmp)) {
                json->create_additions = RTEST(rb_hash_aref(opts, tmp));
            } else {
                json->create_additions = 1;
            }
            tmp = ID2SYM(i_create_id);
            if (option_given_p(opts, tmp)) {
                json->create_id = rb_hash_aref(opts, tmp);
            } else {
                json->create_id = rb_funcall(mJSON, i_create_id, 0);
            }
            tmp = ID2SYM(i_object_class);
            if (option_given_p(opts, tmp)) {
                json->object_class = rb_hash_aref(opts, tmp);
            } else {
                json->object_class = Qnil;
            }
            tmp = ID2SYM(i_array_class);
            if (option_given_p(opts, tmp)) {
                json->array_class = rb_hash_aref(opts, tmp);
            } else {
                json->array_class = Qnil;
            }
            tmp = ID2SYM(i_match_string);
            if (option_given_p(opts, tmp)) {
                VALUE match_string = rb_hash_aref(opts, tmp);
                json->match_string = RTEST(match_string) ? match_string : Qnil;
            } else {
                json->match_string = Qnil;
            }
        }
    } else {
        json->max_nesting = 19;
        json->allow_nan = 0;
        json->create_additions = 1;
        json->create_id = rb_funcall(mJSON, i_create_id, 0);
        json->object_class = Qnil;
        json->array_class = Qnil;
    }
    source = rb_convert_type(source, T_STRING, "String", "to_str");
    if (!json->quirks_mode) {
      source = convert_encoding(StringValue(source));
    }
    json->current_nesting = 0;
    json->len = RSTRING_LEN(source);
    json->source = RSTRING_PTR(source);;
    json->Vsource = source;
    return self;
}

%%{
    machine JSON;

    write data;

    include JSON_common;

    action parse_object {
        char *np;
        json->current_nesting = 1;
        np = JSON_parse_object(json, fpc, pe, &result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    action parse_array {
        char *np;
        json->current_nesting = 1;
        np = JSON_parse_array(json, fpc, pe, &result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    main := ignore* (
            begin_object >parse_object |
            begin_array >parse_array
            ) ignore*;
}%%

static VALUE cParser_parse_strict(VALUE self)
{
    char *p, *pe;
    int cs = EVIL;
    VALUE result = Qnil;
    GET_PARSER;

    %% write init;
    p = json->source;
    pe = p + json->len;
    %% write exec;

    if (cs >= JSON_first_final && p == pe) {
        return result;
    } else {
        rb_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return Qnil;
    }
}


%%{
    machine JSON_quirks_mode;

    write data;

    include JSON_common;

    action parse_value {
        char *np = JSON_parse_value(json, fpc, pe, &result);
        if (np == NULL) { fhold; fbreak; } else fexec np;
    }

    main := ignore* (
            begin_value >parse_value
            ) ignore*;
}%%

static VALUE cParser_parse_quirks_mode(VALUE self)
{
    char *p, *pe;
    int cs = EVIL;
    VALUE result = Qnil;
    GET_PARSER;

    %% write init;
    p = json->source;
    pe = p + json->len;
    %% write exec;

    if (cs >= JSON_quirks_mode_first_final && p == pe) {
        return result;
    } else {
        rb_raise(eParserError, "%u: unexpected token at '%s'", __LINE__, p);
        return Qnil;
    }
}

/*
 * call-seq: parse()
 *
 *  Parses the current JSON text _source_ and returns the complete data
 *  structure as a result.
 */
static VALUE cParser_parse(VALUE self)
{
  GET_PARSER;

  if (json->quirks_mode) {
    return cParser_parse_quirks_mode(self);
  } else {
    return cParser_parse_strict(self);
  }
}


static JSON_Parser *JSON_allocate()
{
    JSON_Parser *json = ALLOC(JSON_Parser);
    MEMZERO(json, JSON_Parser, 1);
    json->fbuffer = fbuffer_alloc(0);
    return json;
}

static void JSON_mark(JSON_Parser *json)
{
    rb_gc_mark_maybe(json->Vsource);
    rb_gc_mark_maybe(json->create_id);
    rb_gc_mark_maybe(json->object_class);
    rb_gc_mark_maybe(json->array_class);
    rb_gc_mark_maybe(json->match_string);
}

static void JSON_free(JSON_Parser *json)
{
    fbuffer_free(json->fbuffer);
    ruby_xfree(json);
}

static VALUE cJSON_parser_s_allocate(VALUE klass)
{
    JSON_Parser *json = JSON_allocate();
    return Data_Wrap_Struct(klass, JSON_mark, JSON_free, json);
}

/*
 * call-seq: source()
 *
 * Returns a copy of the current _source_ string, that was used to construct
 * this Parser.
 */
static VALUE cParser_source(VALUE self)
{
    GET_PARSER;
    return rb_str_dup(json->Vsource);
}

/*
 * call-seq: quirks_mode?()
 *
 * Returns a true, if this parser is in quirks_mode, false otherwise.
 */
static VALUE cParser_quirks_mode_p(VALUE self)
{
    GET_PARSER;
    return json->quirks_mode ? Qtrue : Qfalse;
}


void Init_parser()
{
    rb_require("json/common");
    mJSON = rb_define_module("JSON");
    mExt = rb_define_module_under(mJSON, "Ext");
    cParser = rb_define_class_under(mExt, "Parser", rb_cObject);
    eParserError = rb_path2class("JSON::ParserError");
    eNestingError = rb_path2class("JSON::NestingError");
    rb_define_alloc_func(cParser, cJSON_parser_s_allocate);
    rb_define_method(cParser, "initialize", cParser_initialize, -1);
    rb_define_method(cParser, "parse", cParser_parse, 0);
    rb_define_method(cParser, "source", cParser_source, 0);
    rb_define_method(cParser, "quirks_mode?", cParser_quirks_mode_p, 0);

    CNaN = rb_const_get(mJSON, rb_intern("NaN"));
    CInfinity = rb_const_get(mJSON, rb_intern("Infinity"));
    CMinusInfinity = rb_const_get(mJSON, rb_intern("MinusInfinity"));

    i_json_creatable_p = rb_intern("json_creatable?");
    i_json_create = rb_intern("json_create");
    i_create_id = rb_intern("create_id");
    i_create_additions = rb_intern("create_additions");
    i_chr = rb_intern("chr");
    i_max_nesting = rb_intern("max_nesting");
    i_allow_nan = rb_intern("allow_nan");
    i_symbolize_names = rb_intern("symbolize_names");
    i_quirks_mode = rb_intern("quirks_mode");
    i_object_class = rb_intern("object_class");
    i_array_class = rb_intern("array_class");
    i_match = rb_intern("match");
    i_match_string = rb_intern("match_string");
    i_key_p = rb_intern("key?");
    i_deep_const_get = rb_intern("deep_const_get");
    i_aset = rb_intern("[]=");
    i_aref = rb_intern("[]");
    i_leftshift = rb_intern("<<");
#ifdef HAVE_RUBY_ENCODING_H
    CEncoding_UTF_8 = rb_funcall(rb_path2class("Encoding"), rb_intern("find"), 1, rb_str_new2("utf-8"));
    CEncoding_UTF_16BE = rb_funcall(rb_path2class("Encoding"), rb_intern("find"), 1, rb_str_new2("utf-16be"));
    CEncoding_UTF_16LE = rb_funcall(rb_path2class("Encoding"), rb_intern("find"), 1, rb_str_new2("utf-16le"));
    CEncoding_UTF_32BE = rb_funcall(rb_path2class("Encoding"), rb_intern("find"), 1, rb_str_new2("utf-32be"));
    CEncoding_UTF_32LE = rb_funcall(rb_path2class("Encoding"), rb_intern("find"), 1, rb_str_new2("utf-32le"));
    CEncoding_ASCII_8BIT = rb_funcall(rb_path2class("Encoding"), rb_intern("find"), 1, rb_str_new2("ascii-8bit"));
    i_encoding = rb_intern("encoding");
    i_encode = rb_intern("encode");
#else
    i_iconv = rb_intern("iconv");
#endif
}

/*
 * Local variables:
 * mode: c
 * c-file-style: ruby
 * indent-tabs-mode: nil
 * End:
 */
