/* -*- indent-tabs-mode: nil -*- */
/*
 * rubyext.c
 *
 * $Author$
 *
 * Copyright (C) 2003-2005 why the lucky stiff
 */

#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include "syck.h"
#include <sys/types.h>
#include <time.h>

typedef struct RVALUE {
    union {
#if 0
    struct {
        unsigned long flags;    /* always 0 for freed obj */
        struct RVALUE *next;
    } free;
#endif
    struct RBasic  basic;
    struct RObject object;
    struct RClass  klass;
    /*struct RFloat  flonum;*/
    /*struct RString string;*/
    struct RArray  array;
    /*struct RRegexp regexp;*/
    struct RHash   hash;
    /*struct RData   data;*/
    struct RStruct rstruct;
    /*struct RBignum bignum;*/
    /*struct RFile   file;*/
    } as;
} RVALUE;

typedef struct {
   long hash;
   char *buffer;
   long length;
   long remaining;
   int  printed;
} bytestring_t;

#define RUBY_DOMAIN   "ruby.yaml.org,2002"

/*
 * symbols and constants
 */
static ID s_new, s_utc, s_at, s_to_f, s_to_i, s_read, s_binmode, s_call, s_cmp, s_transfer, s_update, s_dup, s_haskey, s_match, s_keys, s_unpack, s_tr_bang, s_default_set, s_tag_read_class, s_tag_subclasses, s_resolver, s_push, s_emitter, s_level, s_detect_implicit, s_node_import, s_out, s_input, s_intern, s_transform, s_yaml_new, s_yaml_initialize, s_node_export, s_to_yaml, s_write, s_set_resolver, s_each;
static ID s_tags, s_kind, s_name, s_options, s_type_id, s_type_id_set, s_style, s_style_set, s_value, s_value_set, s_parse;
static VALUE sym_model, sym_generic, sym_input, sym_bytecode;
static VALUE sym_scalar, sym_seq, sym_map;
static VALUE sym_1quote, sym_2quote, sym_fold, sym_literal, sym_plain, sym_inline;
static VALUE cDate, cNode, cMap, cSeq, cScalar, cOut, cParser, cResolver, cPrivateType, cDomainType, cYObject, cBadAlias, cDefaultKey, cMergeKey, cEmitter, cDateTime;
static VALUE oDefaultResolver, oGenericResolver;

/*
 * my private collection of numerical oddities.
 */
static double S_zero(void)    { return 0.0; }
static double S_one(void) { return 1.0; }
static double S_inf(void) { return S_one() / S_zero(); }
static double S_nan(void) { return S_zero() / S_zero(); }

static VALUE syck_node_transform( VALUE );

/*
 * handler prototypes
 */
SYMID rb_syck_load_handler _((SyckParser *, SyckNode *));
void rb_syck_err_handler _((SyckParser *, const char *));
SyckNode * rb_syck_bad_anchor_handler _((SyckParser *, char *));
void rb_syck_output_handler _((SyckEmitter *, char *, long));
void rb_syck_emitter_handler _((SyckEmitter *, st_data_t));
int syck_parser_assign_io _((SyckParser *, VALUE *));
VALUE syck_scalar_alloc _((VALUE class));
VALUE syck_seq_alloc _((VALUE class));
VALUE syck_map_alloc _((VALUE class));

struct parser_xtra {
    VALUE data;  /* Borrowed this idea from marshal.c to fix [ruby-core:8067] problem */
    VALUE proc;
    VALUE resolver;
    int taint;
};

struct emitter_xtra {
    VALUE oid;
    VALUE data;
    VALUE port;
};

/*
 * Convert YAML to bytecode
 */
VALUE
rb_syck_compile(VALUE self, VALUE port)
{
    SYMID oid;
    int taint;
    char *ret;
    VALUE bc;
    bytestring_t *sav = NULL;
    void *data = NULL;

    SyckParser *parser = syck_new_parser();
    taint = syck_parser_assign_io(parser, &port);
    syck_parser_handler( parser, syck_yaml2byte_handler );
    syck_parser_error_handler( parser, NULL );
    syck_parser_implicit_typing( parser, 0 );
    syck_parser_taguri_expansion( parser, 0 );
    oid = syck_parse( parser );
    if (!syck_lookup_sym( parser, oid, &data )) {
	rb_raise(rb_eSyntaxError, "root node <%p> not found", (void *)oid);
    }
    sav = data;

    ret = S_ALLOCA_N( char, strlen( sav->buffer ) + 3 );
    ret[0] = '\0';
    strcat( ret, "D\n" );
    strcat( ret, sav->buffer );

    syck_free_parser( parser );

    bc = rb_str_new2( ret );
    if ( taint )      OBJ_TAINT( bc );
    return bc;
}

/*
 * read from io.
 */
long
rb_syck_io_str_read( char *buf, SyckIoStr *str, long max_size, long skip )
{
    long len = 0;

    ASSERT( str != NULL );
    max_size -= skip;

    if ( max_size <= 0 ) max_size = 0;
    else
    {
        /*
         * call io#read.
         */
        VALUE src = (VALUE)str->ptr;
        VALUE n = LONG2NUM(max_size);
        VALUE str2 = rb_funcall2(src, s_read, 1, &n);
        if (!NIL_P(str2))
        {
            StringValue(str2);
            len = RSTRING_LEN(str2);
            memcpy( buf + skip, RSTRING_PTR(str2), len );
        }
    }
    len += skip;
    buf[len] = '\0';
    return len;
}

/*
 * determine: are we reading from a string or io?
 * (returns tainted? boolean)
 */
int
syck_parser_assign_io(SyckParser *parser, VALUE *pport)
{
    int taint = Qtrue;
    VALUE tmp, port = *pport;
    if (!NIL_P(tmp = rb_check_string_type(port))) {
        taint = OBJ_TAINTED(port); /* original taintedness */
        port = tmp;
        syck_parser_str( parser, RSTRING_PTR(port), RSTRING_LEN(port), NULL );
    }
    else if (rb_respond_to(port, s_read)) {
        if (rb_respond_to(port, s_binmode)) {
            rb_funcall2(port, s_binmode, 0, 0);
        }
        syck_parser_str( parser, (char *)port, 0, rb_syck_io_str_read );
    }
    else {
        rb_raise(rb_eTypeError, "instance of IO needed");
    }
    *pport = port;
    return taint;
}

/*
 * Get value in hash by key, forcing an empty hash if nil.
 */
VALUE
syck_get_hash_aref(VALUE hsh, VALUE key)
{
   VALUE val = rb_hash_aref( hsh, key );
   if ( NIL_P( val ) )
   {
       val = rb_hash_new();
       rb_hash_aset(hsh, key, val);
   }
   return val;
}

/*
 * creating timestamps
 */
struct mktime_arg {
    const char *str;
    long len;
};

VALUE
mktime_do(VALUE varg)
{
    struct mktime_arg *arg = (struct mktime_arg *)varg;
    VALUE time;
    const char *str = arg->str;
    long len = arg->len;
    const char *ptr = str;
    VALUE year = INT2FIX(0);
    VALUE mon = INT2FIX(0);
    VALUE day = INT2FIX(0);
    VALUE hour = INT2FIX(0);
    VALUE min = INT2FIX(0);
    VALUE sec = INT2FIX(0);
    long usec;

    /* Year*/
    if ( ptr[0] != '\0' && len > 0 ) {
        year = INT2FIX(strtol(ptr, NULL, 10));
    }

    /* Month*/
    ptr += 4;
    if ( ptr[0] != '\0' && len > ptr - str ) {
        while ( !ISDIGIT( *ptr ) ) ptr++;
        mon = INT2FIX(strtol(ptr, NULL, 10));
    }

    /* Day*/
    ptr += 2;
    if ( ptr[0] != '\0' && len > ptr - str ) {
        while ( !ISDIGIT( *ptr ) ) ptr++;
        day = INT2FIX(strtol(ptr, NULL, 10));
    }

    /* Hour*/
    ptr += 2;
    if ( ptr[0] != '\0' && len > ptr - str ) {
        while ( !ISDIGIT( *ptr ) ) ptr++;
        hour = INT2FIX(strtol(ptr, NULL, 10));
    }

    /* Minute */
    ptr += 2;
    if ( ptr[0] != '\0' && len > ptr - str ) {
        while ( !ISDIGIT( *ptr ) ) ptr++;
        min = INT2FIX(strtol(ptr, NULL, 10));
    }

    /* Second */
    ptr += 2;
    if ( ptr[0] != '\0' && len > ptr - str ) {
        while ( !ISDIGIT( *ptr ) ) ptr++;
        sec = INT2FIX(strtol(ptr, NULL, 10));
    }

    /* Millisecond */
    ptr += 2;
    if ( len > ptr - str && *ptr == '.' )
    {
        char padded[] = "000000";
        const int padding = (int)(sizeof(padded) - 1);
        const char *end = ptr + 1;
        const char *begin = end;
        ptrdiff_t length;
        while ( ISDIGIT( *end ) ) end++;
        if ((length = (end - begin)) > padding) length = padding;
        MEMCPY(padded, begin, char, length);
        usec = strtol(padded, NULL, 10);
    }
    else
    {
        usec = 0;
    }

    /* Time Zone*/
    while ( len > ptr - str && *ptr != 'Z' && *ptr != '+' && *ptr != '-' && *ptr != '\0' ) ptr++;
    if ( len > ptr - str && ( *ptr == '-' || *ptr == '+' ) )
    {
        time_t tz_offset = strtol(ptr, NULL, 10) * 3600;
        VALUE tmp;

        while ( *ptr != ':' && *ptr != '\0' ) ptr++;
        if ( *ptr == ':' )
        {
            ptr += 1;
            if ( tz_offset < 0 )
            {
                tz_offset -= strtol(ptr, NULL, 10) * 60;
            }
            else
            {
                tz_offset += strtol(ptr, NULL, 10) * 60;
            }
        }

        /* Make TZ time*/
        time = rb_funcall(rb_cTime, s_utc, 6, year, mon, day, hour, min, sec);
        tmp = rb_funcall(time, s_to_i, 0);
        tmp = rb_funcall(tmp, '-', 1, LONG2FIX(tz_offset));
        return rb_funcall(rb_cTime, s_at, 2, tmp, LONG2NUM(usec));
    }
    else
    {
        /* Make UTC time*/
        return rb_funcall(rb_cTime, s_utc, 7, year, mon, day, hour, min, sec, LONG2NUM(usec));
    }
}

VALUE
mktime_r(VALUE varg)
{
    struct mktime_arg *arg = (struct mktime_arg *)varg;

    if (!cDateTime) {
        /*
         * Load Date module
         */
        rb_require("date");
        cDateTime = rb_const_get(rb_cObject, rb_intern("DateTime"));
    }
    return rb_funcall(cDateTime, s_parse, 1, rb_str_new(arg->str, arg->len));
}

VALUE
rb_syck_mktime(const char *str, long len)
{
    struct mktime_arg a;

    a.str = str;
    a.len = len;
    return rb_rescue2(mktime_do, (VALUE)&a, mktime_r, (VALUE)&a, rb_eArgError, NULL);
}

/*
 * handles merging of an array of hashes
 * (see http://www.yaml.org/type/merge/)
 */
VALUE
syck_merge_i(VALUE entry, VALUE hsh )
{
    VALUE tmp;
    if ( !NIL_P(tmp = rb_check_convert_type(entry, T_HASH, "Hash", "to_hash")) )
    {
        entry = tmp;
        rb_funcall( hsh, s_update, 1, entry );
    }
    return Qnil;
}

/*
 * default handler for ruby.yaml.org types
 */
int
yaml_org_handler( SyckNode *n, VALUE *ref )
{
    char *type_id = n->type_id;
    int transferred = 0;
    long i = 0;
    VALUE obj = Qnil;

    if ( type_id != NULL && strncmp( type_id, "tag:yaml.org,2002:", 18 ) == 0 )
    {
        type_id += 18;
    }

    switch (n->kind)
    {
        case syck_str_kind:
            transferred = 1;
            if ( type_id == NULL )
            {
                obj = rb_str_new( n->data.str->ptr, n->data.str->len );
            }
            else if ( strcmp( type_id, "null" ) == 0 )
            {
                obj = Qnil;
            }
            else if ( strcmp( type_id, "binary" ) == 0 )
            {
                VALUE arr;
                obj = rb_str_new( n->data.str->ptr, n->data.str->len );
                rb_funcall( obj, s_tr_bang, 2, rb_str_new2( "\n\t " ), rb_str_new2( "" ) );
                arr = rb_funcall( obj, s_unpack, 1, rb_str_new2( "m" ) );
                obj = rb_ary_shift( arr );
            }
            else if ( strcmp( type_id, "bool#yes" ) == 0 )
            {
                obj = Qtrue;
            }
            else if ( strcmp( type_id, "bool#no" ) == 0 )
            {
                obj = Qfalse;
            }
            else if ( strcmp( type_id, "int#hex" ) == 0 )
            {
                syck_str_blow_away_commas( n );
                obj = rb_cstr2inum( n->data.str->ptr, 16 );
            }
            else if ( strcmp( type_id, "int#oct" ) == 0 )
            {
                syck_str_blow_away_commas( n );
                obj = rb_cstr2inum( n->data.str->ptr, 8 );
            }
            else if ( strcmp( type_id, "int#base60" ) == 0 )
            {
                char *ptr, *end;
                long sixty = 1;
                long total = 0;
                syck_str_blow_away_commas( n );
                ptr = n->data.str->ptr;
                end = n->data.str->ptr + n->data.str->len;
                while ( end > ptr )
                {
                    long bnum = 0;
                    char *colon = end - 1;
                    while ( colon >= ptr && *colon != ':' )
                    {
                        colon--;
                    }
                    if ( colon >= ptr && *colon == ':' ) *colon = '\0';

                    bnum = strtol( colon + 1, NULL, 10 );
                    total += bnum * sixty;
                    sixty *= 60;
                    end = colon;
                }
                obj = INT2FIX(total);
            }
            else if ( strncmp( type_id, "int", 3 ) == 0 )
            {
                syck_str_blow_away_commas( n );
                obj = rb_cstr2inum( n->data.str->ptr, 10 );
            }
            else if ( strcmp( type_id, "float#base60" ) == 0 )
            {
                char *ptr, *end;
                long sixty = 1;
                double total = 0.0;
                syck_str_blow_away_commas( n );
                ptr = n->data.str->ptr;
                end = n->data.str->ptr + n->data.str->len;
                while ( end > ptr )
                {
                    double bnum = 0;
                    char *colon = end - 1;
                    while ( colon >= ptr && *colon != ':' )
                    {
                        colon--;
                    }
                    if ( colon >= ptr && *colon == ':' ) *colon = '\0';

                    bnum = strtod( colon + 1, NULL );
                    total += bnum * sixty;
                    sixty *= 60;
                    end = colon;
                }
                obj = rb_float_new( total );
            }
            else if ( strcmp( type_id, "float#nan" ) == 0 )
            {
                obj = rb_float_new( S_nan() );
            }
            else if ( strcmp( type_id, "float#inf" ) == 0 )
            {
                obj = rb_float_new( S_inf() );
            }
            else if ( strcmp( type_id, "float#neginf" ) == 0 )
            {
                obj = rb_float_new( -S_inf() );
            }
            else if ( strncmp( type_id, "float", 5 ) == 0 )
            {
                double f;
                syck_str_blow_away_commas( n );
                f = strtod( n->data.str->ptr, NULL );
                obj = rb_float_new( f );
            }
            else if ( strcmp( type_id, "timestamp#iso8601" ) == 0 )
            {
                obj = rb_syck_mktime( n->data.str->ptr, n->data.str->len );
            }
            else if ( strcmp( type_id, "timestamp#spaced" ) == 0 )
            {
                obj = rb_syck_mktime( n->data.str->ptr, n->data.str->len );
            }
            else if ( strcmp( type_id, "timestamp#ymd" ) == 0 )
            {
                char *ptr = n->data.str->ptr;
                VALUE year, mon, day;

                /* Year*/
                ptr[4] = '\0';
                year = INT2FIX(strtol(ptr, NULL, 10));

                /* Month*/
                ptr += 4;
                while ( !ISDIGIT( *ptr ) ) ptr++;
                mon = INT2FIX(strtol(ptr, NULL, 10));

                /* Day*/
                ptr += 2;
                while ( !ISDIGIT( *ptr ) ) ptr++;
                day = INT2FIX(strtol(ptr, NULL, 10));

                if ( !cDate ) {
                    /*
                     * Load Date module
                     */
                    rb_require( "date" );
                    cDate = rb_const_get( rb_cObject, rb_intern("Date") );
                }

                obj = rb_funcall( cDate, s_new, 3, year, mon, day );
            }
            else if ( strncmp( type_id, "timestamp", 9 ) == 0 )
            {
                obj = rb_syck_mktime( n->data.str->ptr, n->data.str->len );
            }
            else if ( strncmp( type_id, "merge", 5 ) == 0 )
            {
                obj = rb_funcall( cMergeKey, s_new, 0 );
            }
            else if ( strncmp( type_id, "default", 7 ) == 0 )
            {
                obj = rb_funcall( cDefaultKey, s_new, 0 );
            }
            else if ( n->data.str->style == scalar_plain &&
                      n->data.str->len > 1 &&
                      strncmp( n->data.str->ptr, ":", 1 ) == 0 )
            {
                obj = rb_funcall( oDefaultResolver, s_transfer, 2,
                                  rb_str_new2( "tag:ruby.yaml.org,2002:sym" ),
                                  rb_str_new( n->data.str->ptr + 1, n->data.str->len - 1 ) );
            }
            else if ( strcmp( type_id, "str" ) == 0 )
            {
                obj = rb_str_new( n->data.str->ptr, n->data.str->len );
                rb_enc_associate(obj, rb_utf8_encoding());
            }
            else
            {
                transferred = 0;
                obj = rb_str_new( n->data.str->ptr, n->data.str->len );
            }
        break;

        case syck_seq_kind:
            if ( type_id == NULL || strcmp( type_id, "seq" ) == 0 )
            {
                transferred = 1;
            }
            obj = rb_ary_new2( n->data.list->idx );
            for ( i = 0; i < n->data.list->idx; i++ )
            {
                rb_ary_store( obj, i, syck_seq_read( n, i ) );
            }
        break;

        case syck_map_kind:
            if ( type_id == NULL || strcmp( type_id, "map" ) == 0 )
            {
                transferred = 1;
            }
            obj = rb_hash_new();
            for ( i = 0; i < n->data.pairs->idx; i++ )
            {
                VALUE k = syck_map_read( n, map_key, i );
                VALUE v = syck_map_read( n, map_value, i );
                int skip_aset = 0;

                /*
                 * Handle merge keys
                 */
                if ( rb_obj_is_kind_of( k, cMergeKey ) )
                {
                    VALUE tmp;
                    if ( !NIL_P(tmp = rb_check_convert_type(v, T_HASH, "Hash", "to_hash")) )
                    {
                        VALUE dup = rb_funcall( tmp, s_dup, 0 );
                        rb_funcall( dup, s_update, 1, obj );
                        obj = dup;
                        skip_aset = 1;
                    }
                    else if ( !NIL_P(tmp = rb_check_array_type(v)) )
                    {
                        VALUE end = rb_ary_pop( tmp );
                        VALUE tmph = rb_check_convert_type(end, T_HASH, "Hash", "to_hash");
                        if ( !NIL_P(tmph) )
                        {
                            VALUE dup = rb_funcall( tmph, s_dup, 0 );
                            tmp = rb_ary_reverse( tmp );
                            rb_ary_push( tmp, obj );
                            rb_block_call( tmp, s_each, 0, 0, syck_merge_i, dup );
                            obj = dup;
                            skip_aset = 1;
                        }
                    }
                }
                else if ( rb_obj_is_kind_of( k, cDefaultKey ) )
                {
                    rb_funcall( obj, s_default_set, 1, v );
                    skip_aset = 1;
                }

                if ( ! skip_aset )
                {
                    rb_hash_aset( obj, k, v );
                }
            }
        break;
    }

    *ref = obj;
    return transferred;
}

static void syck_node_mark( SyckNode *n );

/*
 * {native mode} node handler
 * - Converts data into native Ruby types
 */
SYMID
rb_syck_load_handler(SyckParser *p, SyckNode *n)
{
    VALUE obj = Qnil;
    struct parser_xtra *bonus = (struct parser_xtra *)p->bonus;
    VALUE resolver = bonus->resolver;
    if ( NIL_P( resolver ) )
    {
        resolver = oDefaultResolver;
    }

    /*
     * Create node,
     */
    obj = rb_funcall( resolver, s_node_import, 1, Data_Wrap_Struct( cNode, NULL, NULL, n ) );

    /*
     * ID already set, let's alter the symbol table to accept the new object
     */
    if (n->id > 0 && !NIL_P(obj))
    {
        MEMCPY((void *)n->id, (void *)obj, RVALUE, 1);
        MEMZERO((void *)obj, RVALUE, 1);
        obj = n->id;
    }

    if ( bonus->taint)      OBJ_TAINT( obj );
    if ( bonus->proc != 0 ) rb_funcall(bonus->proc, s_call, 1, obj);

    rb_hash_aset(bonus->data, INT2FIX(RHASH_SIZE(bonus->data)), obj);
    return obj;
}

/*
 * friendly errors.
 */
void
rb_syck_err_handler(SyckParser *p, const char *msg)
{
    char *endl = p->cursor;

    while ( *endl != '\0' && *endl != '\n' )
        endl++;

    endl[0] = '\0';
    rb_raise(rb_eArgError, "%s on line %d, col %"PRIdPTRDIFF": `%s'",
           msg,
           p->linect,
           p->cursor - p->lineptr,
           p->lineptr);
}

/*
 * provide bad anchor object to the parser.
 */
SyckNode *
rb_syck_bad_anchor_handler(SyckParser *p, char *a)
{
    VALUE anchor_name = rb_str_new2( a );
    SyckNode *badanc = syck_new_map( rb_str_new2( "name" ), anchor_name );
    badanc->type_id = syck_strndup( "tag:ruby.yaml.org,2002:object:YAML::Syck::BadAlias", 53 );
    return badanc;
}

/*
 * data loaded based on the model requested.
 */
void
syck_set_model(VALUE p, VALUE input, VALUE model)
{
    SyckParser *parser;
    Data_Get_Struct(p, SyckParser, parser);
    syck_parser_handler( parser, rb_syck_load_handler );
    /* WARN: gonna be obsoleted soon!! */
    if ( model == sym_generic )
    {
        rb_funcall( p, s_set_resolver, 1, oGenericResolver );
    }
    syck_parser_implicit_typing( parser, 1 );
    syck_parser_taguri_expansion( parser, 1 );

    if ( NIL_P( input ) )
    {
        input = rb_ivar_get( p, s_input );
    }
    if ( input == sym_bytecode )
    {
        syck_parser_set_input_type( parser, syck_bytecode_utf8 );
    }
    else
    {
        syck_parser_set_input_type( parser, syck_yaml_utf8 );
    }
    syck_parser_error_handler( parser, rb_syck_err_handler );
    syck_parser_bad_anchor_handler( parser, rb_syck_bad_anchor_handler );
}

static int
syck_st_mark_nodes( char *key, SyckNode *n, char *arg )
{
    if ( n != (void *)1 ) syck_node_mark( n );
    return ST_CONTINUE;
}

/*
 * mark parser nodes
 */
static void
syck_mark_parser(SyckParser *parser)
{
    struct parser_xtra *bonus = (struct parser_xtra *)parser->bonus;
    rb_gc_mark_maybe(parser->root);
    rb_gc_mark_maybe(parser->root_on_error);
    rb_gc_mark( bonus->data );
    rb_gc_mark( bonus->proc );
    rb_gc_mark( bonus->resolver );

    if ( parser->anchors != NULL )
    {
        st_foreach( parser->anchors, syck_st_mark_nodes, 0 );
    }
    if ( parser->bad_anchors != NULL )
    {
        st_foreach( parser->bad_anchors, syck_st_mark_nodes, 0 );
    }
}

/*
 * Free the parser and any bonus attachment.
 */
void
rb_syck_free_parser(SyckParser *p)
{
    S_FREE( p->bonus );
    syck_free_parser(p);
}

/*
 * YAML::Syck::Parser.allocate
 */
VALUE syck_parser_s_alloc _((VALUE));
VALUE
syck_parser_s_alloc(VALUE class)
{
    VALUE pobj;
    SyckParser *parser = syck_new_parser();

    parser->bonus = S_ALLOC( struct parser_xtra );
    S_MEMZERO( parser->bonus, struct parser_xtra, 1 );

    pobj = Data_Wrap_Struct( class, syck_mark_parser, rb_syck_free_parser, parser );

    syck_parser_set_root_on_error( parser, Qnil );

    return pobj;
}

/*
 * YAML::Syck::Parser.initialize( resolver, options )
 */
static VALUE
syck_parser_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE options;
    if (rb_scan_args(argc, argv, "01", &options) == 0)
    {
        options = rb_hash_new();
    }
    else
    {
        Check_Type(options, T_HASH);
    }
    rb_ivar_set(self, s_options, options);
    rb_ivar_set(self, s_input, Qnil);
    return self;
}

/*
 * YAML::Syck::Parser.bufsize = Integer
 */
static VALUE
syck_parser_bufsize_set(VALUE self, VALUE size)
{
    SyckParser *parser;

    if ( rb_respond_to( size, s_to_i ) ) {
        int n = NUM2INT(rb_funcall(size, s_to_i, 0));
        Data_Get_Struct(self, SyckParser, parser);
        parser->bufsize = n;
    }
    return self;
}

/*
 * YAML::Syck::Parser.bufsize => Integer
 */
static VALUE
syck_parser_bufsize_get(VALUE self)
{
    SyckParser *parser;

    Data_Get_Struct(self, SyckParser, parser);
    return INT2FIX( parser->bufsize );
}

/*
 * YAML::Syck::Parser.load( IO or String )
 */
VALUE
syck_parser_load(int argc, VALUE *argv, VALUE self)
{
    VALUE port, proc, model, input;
    SyckParser *parser;
    struct parser_xtra *bonus;

    rb_scan_args(argc, argv, "11", &port, &proc);

    input = rb_hash_aref( rb_attr_get( self, s_options ), sym_input );
    model = rb_hash_aref( rb_attr_get( self, s_options ), sym_model );
    Data_Get_Struct(self, SyckParser, parser);
    syck_set_model( self, input, model );

    bonus = (struct parser_xtra *)parser->bonus;
    bonus->taint = syck_parser_assign_io(parser, &port);
    bonus->data = rb_hash_new();
    bonus->resolver = rb_attr_get( self, s_resolver );
    if ( NIL_P( proc ) ) bonus->proc = 0;
    else                 bonus->proc = proc;

    return syck_parse( parser );
}

/*
 * YAML::Syck::Parser.load_documents( IO or String ) { |doc| }
 */
VALUE
syck_parser_load_documents(int argc, VALUE *argv, VALUE self)
{
    VALUE port, proc, v, input, model;
    SyckParser *parser;
    struct parser_xtra *bonus;

    rb_scan_args(argc, argv, "1&", &port, &proc);

    input = rb_hash_aref( rb_attr_get( self, s_options ), sym_input );
    model = rb_hash_aref( rb_attr_get( self, s_options ), sym_model );
    Data_Get_Struct(self, SyckParser, parser);
    syck_set_model( self, input, model );

    bonus = (struct parser_xtra *)parser->bonus;
    bonus->taint = syck_parser_assign_io(parser, &port);
    bonus->resolver = rb_attr_get( self, s_resolver );
    bonus->proc = 0;

    while ( 1 )
    {
        /* Reset hash for tracking nodes */
        bonus->data = rb_hash_new();

        /* Parse a document */
        v = syck_parse( parser );
        if ( parser->eof == 1 )
        {
            break;
        }

        /* Pass document to block */
        rb_funcall( proc, s_call, 1, v );
    }

    return Qnil;
}

/*
 * YAML::Syck::Parser#set_resolver
 */
VALUE
syck_parser_set_resolver(VALUE self, VALUE resolver)
{
    rb_ivar_set( self, s_resolver, resolver );
    return self;
}

/*
 * YAML::Syck::Resolver.initialize
 */
static VALUE
syck_resolver_initialize(VALUE self)
{
    rb_ivar_set(self, s_tags, rb_hash_new());
    return self;
}

/*
 * YAML::Syck::Resolver#add_type
 */
VALUE
syck_resolver_add_type(VALUE self, VALUE taguri, VALUE cls)
{
    VALUE tags = rb_attr_get(self, s_tags);
    rb_hash_aset( tags, taguri, cls );
    return Qnil;
}

/*
 * YAML::Syck::Resolver#use_types_at
 */
VALUE
syck_resolver_use_types_at(VALUE self, VALUE hsh)
{
    rb_ivar_set( self, s_tags, hsh );
    return Qnil;
}

/*
 * YAML::Syck::Resolver#detect_implicit
 */
VALUE
syck_resolver_detect_implicit(VALUE self, VALUE val)
{
    return rb_str_new2( "" );
}

/*
 * YAML::Syck::Resolver#node_import
 */
VALUE
syck_resolver_node_import(VALUE self, VALUE node)
{
    SyckNode *n;
    VALUE obj = Qnil;
    int i = 0;
    Data_Get_Struct(node, SyckNode, n);

    switch (n->kind)
    {
        case syck_str_kind:
            obj = rb_str_new( n->data.str->ptr, n->data.str->len );
        break;

        case syck_seq_kind:
            obj = rb_ary_new2( n->data.list->idx );
            for ( i = 0; i < n->data.list->idx; i++ )
            {
                rb_ary_store( obj, i, syck_seq_read( n, i ) );
            }
        break;

        case syck_map_kind:
            obj = rb_hash_new();
            for ( i = 0; i < n->data.pairs->idx; i++ )
            {
                VALUE k = syck_map_read( n, map_key, i );
                VALUE v = syck_map_read( n, map_value, i );
                int skip_aset = 0;

                /*
                 * Handle merge keys
                 */
                if ( rb_obj_is_kind_of( k, cMergeKey ) )
                {
                    if ( rb_obj_is_kind_of( v, rb_cHash ) )
                    {
                        VALUE dup = rb_funcall( v, s_dup, 0 );
                        rb_funcall( dup, s_update, 1, obj );
                        obj = dup;
                        skip_aset = 1;
                    }
                    else if ( rb_obj_is_kind_of( v, rb_cArray ) )
                    {
                        VALUE end = rb_ary_pop( v );
                        if ( rb_obj_is_kind_of( end, rb_cHash ) )
                        {
                            VALUE dup = rb_funcall( end, s_dup, 0 );
                            v = rb_ary_reverse( v );
                            rb_ary_push( v, obj );
                            rb_block_call( v, s_each, 0, 0, syck_merge_i, dup );
                            obj = dup;
                            skip_aset = 1;
                        }
                    }
                }
                else if ( rb_obj_is_kind_of( k, cDefaultKey ) )
                {
                    rb_funcall( obj, s_default_set, 1, v );
                    skip_aset = 1;
                }

                if ( ! skip_aset )
                {
                    rb_hash_aset( obj, k, v );
                }
            }
        break;
    }

    if ( n->type_id != NULL )
    {
        obj = rb_funcall( self, s_transfer, 2, rb_str_new2( n->type_id ), obj );
    }
    return obj;
}

/*
 * Set instance variables
 */
VALUE
syck_set_ivars(VALUE vars, VALUE obj)
{
    VALUE ivname = rb_ary_entry( vars, 0 );
    char *ivn;
    StringValue( ivname );
    ivn = S_ALLOCA_N( char, RSTRING_LEN(ivname) + 2 );
    ivn[0] = '@';
    ivn[1] = '\0';
    strncat( ivn, RSTRING_PTR(ivname), RSTRING_LEN(ivname) );
    rb_iv_set( obj, ivn, rb_ary_entry( vars, 1 ) );
    return Qnil;
}

/*
 * YAML::Syck::Resolver#const_find
 */
VALUE
syck_const_find(VALUE const_name)
{
    VALUE tclass = rb_cObject;
    VALUE tparts = rb_str_split( const_name, "::" );
    int i = 0;
    for ( i = 0; i < RARRAY_LEN(tparts); i++ ) {
        VALUE tpart = rb_to_id( rb_ary_entry( tparts, i ) );
        if ( !rb_const_defined( tclass, tpart ) ) return Qnil;
        tclass = rb_const_get( tclass, tpart );
    }
    return tclass;
}

/*
 * YAML::Syck::Resolver#transfer
 */
VALUE
syck_resolver_transfer(VALUE self, VALUE type, VALUE val)
{
    if (NIL_P(type) || RSTRING_LEN(StringValue(type)) == 0)
    {
        type = rb_funcall( self, s_detect_implicit, 1, val );
    }

    if ( ! (NIL_P(type) || RSTRING_LEN(StringValue(type)) == 0) )
    {
        VALUE str_xprivate = rb_str_new2( "x-private" );
        VALUE colon = rb_str_new2( ":" );
        VALUE tags = rb_attr_get(self, s_tags);
        VALUE target_class = rb_hash_aref( tags, type );
        VALUE subclass = target_class;
        VALUE obj = Qnil;

        /*
         * Should no tag match exactly, check for subclass format
         */
        if ( NIL_P( target_class ) )
        {
            VALUE subclass_parts = rb_ary_new();
            VALUE parts = rb_str_split( type, ":" );

            while ( RARRAY_LEN(parts) > 1 )
            {
                VALUE partial;
                rb_ary_unshift( subclass_parts, rb_ary_pop( parts ) );
                partial = rb_ary_join( parts, colon );
                target_class = rb_hash_aref( tags, partial );
                if ( NIL_P( target_class ) )
                {
                    rb_str_append( partial, colon );
                    target_class = rb_hash_aref( tags, partial );
                }

                /*
                 * Possible subclass found, see if it supports subclassing
                 */
                if ( ! NIL_P( target_class ) )
                {
                    subclass = target_class;
                    if ( RARRAY_LEN(subclass_parts) > 0 && rb_respond_to( target_class, s_tag_subclasses ) &&
                         RTEST( rb_funcall( target_class, s_tag_subclasses, 0 ) ) )
                    {
                        VALUE subclass_v;
                        subclass = rb_ary_join( subclass_parts, colon );
                        subclass = rb_funcall( target_class, s_tag_read_class, 1, subclass );
                        subclass_v = syck_const_find( subclass );

                        if ( subclass_v != Qnil )
                        {
                            subclass = subclass_v;
                        }
                        else if ( rb_cObject == target_class && subclass_v == Qnil )
                        {
                            target_class = cYObject;
                            type = subclass;
                            subclass = cYObject;
                        }
                        else /* workaround for SEGV. real fix please */
                        {
                            rb_raise( rb_eTypeError, "invalid subclass" );
                        }
                    }
                    break;
                }
            }
        }

        /* rb_raise(rb_eTypeError, "invalid typing scheme: %s given",
         *         scheme);
         */

        if ( rb_respond_to( target_class, s_call ) )
        {
            obj = rb_funcall( target_class, s_call, 2, type, val );
        }
        else
        {
            if ( rb_respond_to( target_class, s_yaml_new ) )
            {
                obj = rb_funcall( target_class, s_yaml_new, 3, subclass, type, val );
            }
            else if ( !NIL_P( target_class ) )
            {
                if ( subclass == rb_cBignum )
                {
                    obj = rb_str2inum( val, 10 ); /* for yaml dumped by 1.8.3 [ruby-core:6159] */
                }
                else
                {
                    obj = rb_obj_alloc( subclass );
                }

                if ( rb_respond_to( obj, s_yaml_initialize ) )
                {
                    rb_funcall( obj, s_yaml_initialize, 2, type, val );
                }
                else if ( !NIL_P( obj ) && rb_obj_is_instance_of( val, rb_cHash ) )
                {
                    rb_block_call( val, s_each, 0, 0, syck_set_ivars, obj );
                }
            }
            else
            {
                VALUE parts = rb_str_split( type, ":" );
                VALUE scheme = rb_ary_shift( parts );
                if ( rb_str_cmp( scheme, str_xprivate ) == 0 )
                {
                    VALUE name = rb_ary_join( parts, colon );
                    obj = rb_funcall( cPrivateType, s_new, 2, name, val );
                }
                else
                {
                    VALUE domain = rb_ary_shift( parts );
                    VALUE name = rb_ary_join( parts, colon );
                    obj = rb_funcall( cDomainType, s_new, 3, domain, name, val );
                }
            }
        }
        val = obj;
    }

    return val;
}

/*
 * YAML::Syck::Resolver#tagurize
 */
VALUE
syck_resolver_tagurize(VALUE self, VALUE val)
{
    VALUE tmp = rb_check_string_type(val);

    if ( !NIL_P(tmp) )
    {
        char *taguri = syck_type_id_to_uri( RSTRING_PTR(tmp) );
        val = rb_str_new2( taguri );
        S_FREE( taguri );
    }

    return val;
}

/*
 * YAML::Syck::DefaultResolver#detect_implicit
 */
VALUE
syck_defaultresolver_detect_implicit(VALUE self, VALUE val)
{
    const char *type_id;
    VALUE tmp = rb_check_string_type(val);

    if ( !NIL_P(tmp) )
    {
        val = tmp;
        type_id = syck_match_implicit( RSTRING_PTR(val), RSTRING_LEN(val) );
        return rb_str_new2( type_id );
    }

    return rb_str_new2( "" );
}

/*
 * YAML::Syck::DefaultResolver#node_import
 */
VALUE
syck_defaultresolver_node_import(VALUE self, VALUE node)
{
    SyckNode *n;
    VALUE obj;
    Data_Get_Struct( node, SyckNode, n );
    if ( !yaml_org_handler( n, &obj ) )
    {
        obj = rb_funcall( self, s_transfer, 2, rb_str_new2( n->type_id ), obj );
    }
    return obj;
}

/*
 * YAML::Syck::GenericResolver#node_import
 */
VALUE
syck_genericresolver_node_import(VALUE self, VALUE node)
{
    SyckNode *n;
    int i = 0;
    VALUE t = Qnil, obj = Qnil, v = Qnil, style = Qnil;
    Data_Get_Struct(node, SyckNode, n);

    if ( n->type_id != NULL )
    {
        t = rb_str_new2(n->type_id);
    }

    switch (n->kind)
    {
        case syck_str_kind:
        {
            v = rb_str_new( n->data.str->ptr, n->data.str->len );
	    rb_enc_associate(v, rb_utf8_encoding());
            if ( n->data.str->style == scalar_1quote )
            {
                style = sym_1quote;
            }
            else if ( n->data.str->style == scalar_2quote )
            {
                style = sym_2quote;
            }
            else if ( n->data.str->style == scalar_fold )
            {
                style = sym_fold;
            }
            else if ( n->data.str->style == scalar_literal )
            {
                style = sym_literal;
            }
            else if ( n->data.str->style == scalar_plain )
            {
                style = sym_plain;
            }
            obj = rb_funcall( cScalar, s_new, 3, t, v, style );
        }
        break;

        case syck_seq_kind:
            v = rb_ary_new2( syck_seq_count( n ) );
            for ( i = 0; i < syck_seq_count( n ); i++ )
            {
                rb_ary_store( v, i, syck_seq_read( n, i ) );
            }
            if ( n->data.list->style == seq_inline )
            {
                style = sym_inline;
            }
            obj = rb_funcall( cSeq, s_new, 3, t, v, style );
            rb_iv_set(obj, "@kind", sym_seq);
        break;

        case syck_map_kind:
            v = rb_hash_new();
            for ( i = 0; i < syck_map_count( n ); i++ )
            {
                rb_hash_aset( v, syck_map_read( n, map_key, i ), syck_map_read( n, map_value, i ) );
            }
            if ( n->data.pairs->style == map_inline )
            {
                style = sym_inline;
            }
            obj = rb_funcall( cMap, s_new, 3, t, v, style );
            rb_iv_set(obj, "@kind", sym_map);
        break;
    }

    return obj;
}

/*
 * YAML::Syck::BadAlias.initialize
 */
VALUE
syck_badalias_initialize(VALUE self, VALUE val)
{
    rb_iv_set( self, "@name", val );
    return self;
}

/*
 * YAML::Syck::BadAlias.<=>
 */
VALUE
syck_badalias_cmp(VALUE alias1, VALUE alias2)
{
    VALUE str1 = rb_ivar_get( alias1, s_name );
    VALUE str2 = rb_ivar_get( alias2, s_name );
    VALUE val = rb_funcall( str1, s_cmp, 1, str2 );
    return val;
}

/*
 * YAML::DomainType.initialize
 */
VALUE
syck_domaintype_initialize(VALUE self, VALUE domain, VALUE type_id, VALUE val)
{
    rb_iv_set( self, "@domain", domain );
    rb_iv_set( self, "@type_id", type_id );
    rb_iv_set( self, "@value", val );
    return self;
}

/*
 * YAML::Object.initialize
 */
VALUE
syck_yobject_initialize(VALUE self, VALUE klass, VALUE ivars)
{
    rb_iv_set( self, "@class", klass );
    rb_iv_set( self, "@ivars", ivars );
    return self;
}

/*
 * YAML::PrivateType.initialize
 */
VALUE
syck_privatetype_initialize(VALUE self, VALUE type_id, VALUE val)
{
    rb_iv_set( self, "@type_id", type_id );
    rb_iv_set( self, "@value", val );
    return self;
}

/*
 * Mark node contents.
 */
static void
syck_node_mark(SyckNode *n)
{
    int i;
    rb_gc_mark_maybe( n->id );
    switch ( n->kind )
    {
        case syck_seq_kind:
            for ( i = 0; i < n->data.list->idx; i++ )
            {
                rb_gc_mark( syck_seq_read( n, i ) );
            }
        break;

        case syck_map_kind:
            for ( i = 0; i < n->data.pairs->idx; i++ )
            {
                rb_gc_mark( syck_map_read( n, map_key, i ) );
                rb_gc_mark( syck_map_read( n, map_value, i ) );
            }
        break;

	case syck_str_kind:
	default:
	    /* nothing */
	break;
    }
#if 0 /* maybe needed */
    if ( n->shortcut ) syck_node_mark( n->shortcut ); /* caution: maybe cyclic */
#endif
}

/*
 * YAML::Syck::Scalar.allocate
 */
VALUE
syck_scalar_alloc(VALUE class)
{
    SyckNode *node = syck_alloc_str();
    VALUE obj = Data_Wrap_Struct( class, syck_node_mark, syck_free_node, node );
    node->id = obj;
    return obj;
}

/*
 * YAML::Syck::Scalar.initialize
 */
VALUE
syck_scalar_initialize(VALUE self, VALUE type_id, VALUE val, VALUE style)
{
    rb_iv_set( self, "@kind", sym_scalar );
    rb_funcall( self, s_type_id_set, 1, type_id );
    rb_funcall( self, s_value_set, 1, val );
    rb_funcall( self, s_style_set, 1, style );
    return self;
}

/*
 * YAML::Syck::Scalar.style=
 */
VALUE
syck_scalar_style_set(VALUE self, VALUE style)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    if ( NIL_P( style ) )
    {
        node->data.str->style = scalar_none;
    }
    else if ( style == sym_1quote )
    {
        node->data.str->style = scalar_1quote;
    }
    else if ( style == sym_2quote )
    {
        node->data.str->style = scalar_2quote;
    }
    else if ( style == sym_fold )
    {
        node->data.str->style = scalar_fold;
    }
    else if ( style == sym_literal )
    {
        node->data.str->style = scalar_literal;
    }
    else if ( style == sym_plain )
    {
        node->data.str->style = scalar_plain;
    }

    rb_iv_set( self, "@style", style );
    return self;
}

/*
 * YAML::Syck::Scalar.value=
 */
VALUE
syck_scalar_value_set(VALUE  self, VALUE val)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    StringValue( val );
    node->data.str->ptr = syck_strndup( RSTRING_PTR(val), RSTRING_LEN(val) );
    node->data.str->len = RSTRING_LEN(val);
    node->data.str->style = scalar_none;

    rb_iv_set( self, "@value", val );
    return val;
}

/*
 * YAML::Syck::Seq.allocate
 */
VALUE
syck_seq_alloc(VALUE class)
{
    SyckNode *node;
    VALUE obj;
    node = syck_alloc_seq();
    obj = Data_Wrap_Struct( class, syck_node_mark, syck_free_node, node );
    node->id = obj;
    return obj;
}

/*
 * YAML::Syck::Seq.initialize
 */
VALUE
syck_seq_initialize(VALUE self, VALUE type_id, VALUE val, VALUE style)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    rb_iv_set( self, "@kind", sym_seq );
    rb_funcall( self, s_type_id_set, 1, type_id );
    rb_funcall( self, s_value_set, 1, val );
    rb_funcall( self, s_style_set, 1, style );
    return self;
}

/*
 * YAML::Syck::Seq.value=
 */
VALUE
syck_seq_value_set(VALUE self, VALUE val)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    val = rb_check_array_type( val );
    if ( !NIL_P( val ) ) {
        int i;
        syck_seq_empty( node );
        for ( i = 0; i < RARRAY_LEN( val ); i++ )
        {
            syck_seq_add( node, rb_ary_entry(val, i) );
        }
    }

    rb_iv_set( self, "@value", val );
    return val;
}

/*
 * YAML::Syck::Seq.add
 */
VALUE
syck_seq_add_m(VALUE self, VALUE val)
{
    SyckNode *node;
    VALUE emitter = rb_ivar_get( self, s_emitter );
    Data_Get_Struct( self, SyckNode, node );

    if ( rb_respond_to( emitter, s_node_export ) ) {
        val = rb_funcall( emitter, s_node_export, 1, val );
    }
    syck_seq_add( node, val );
    rb_ary_push( rb_ivar_get( self, s_value ), val );

    return self;
}

/*
 * YAML::Syck::Seq.style=
 */
VALUE
syck_seq_style_set(VALUE self, VALUE style)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    if ( style == sym_inline )
    {
        node->data.list->style = seq_inline;
    }
    else
    {
        node->data.list->style = seq_none;
    }

    rb_iv_set( self, "@style", style );
    return self;
}

/*
 * YAML::Syck::Map.allocate
 */
VALUE
syck_map_alloc(VALUE class)
{
    SyckNode *node;
    VALUE obj;
    node = syck_alloc_map();
    obj = Data_Wrap_Struct( class, syck_node_mark, syck_free_node, node );
    node->id = obj;
    return obj;
}

/*
 * YAML::Syck::Map.initialize
 */
VALUE
syck_map_initialize(VALUE self, VALUE type_id, VALUE val, VALUE style)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    if ( !NIL_P( val ) )
    {
        VALUE hsh = rb_check_convert_type(val, T_HASH, "Hash", "to_hash");
        VALUE keys;
        int i;
        if ( NIL_P(hsh) )
        {
            rb_raise( rb_eTypeError, "wrong argument type" );
        }

        keys = rb_funcall( hsh, s_keys, 0 );
        for ( i = 0; i < RARRAY_LEN(keys); i++ )
        {
            VALUE key = rb_ary_entry(keys, i);
            syck_map_add( node, key, rb_hash_aref(hsh, key) );
        }
    }

    rb_iv_set( self, "@kind", sym_seq );
    rb_funcall( self, s_type_id_set, 1, type_id );
    rb_funcall( self, s_value_set, 1, val );
    rb_funcall( self, s_style_set, 1, style );
    return self;
}

/*
 * YAML::Syck::Map.value=
 */
VALUE
syck_map_value_set(VALUE self, VALUE val)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    if ( !NIL_P( val ) )
    {
        VALUE hsh = rb_check_convert_type(val, T_HASH, "Hash", "to_hash");
        VALUE keys;
        int i;
        if ( NIL_P(hsh) )
        {
            rb_raise( rb_eTypeError, "wrong argument type" );
        }

        syck_map_empty( node );
        keys = rb_funcall( hsh, s_keys, 0 );
        for ( i = 0; i < RARRAY_LEN(keys); i++ )
        {
            VALUE key = rb_ary_entry(keys, i);
            syck_map_add( node, key, rb_hash_aref(hsh, key) );
        }
    }

    rb_iv_set( self, "@value", val );
    return val;
}

/*
 * YAML::Syck::Map.add
 */
VALUE
syck_map_add_m(VALUE self, VALUE key, VALUE val)
{
    SyckNode *node;
    VALUE emitter = rb_ivar_get( self, s_emitter );
    Data_Get_Struct( self, SyckNode, node );

    if ( rb_respond_to( emitter, s_node_export ) ) {
        key = rb_funcall( emitter, s_node_export, 1, key );
        val = rb_funcall( emitter, s_node_export, 1, val );
    }
    syck_map_add( node, key, val );
    rb_hash_aset( rb_ivar_get( self, s_value ), key, val );

    return self;
}

/*
 * YAML::Syck::Map.style=
 */
VALUE
syck_map_style_set(VALUE self, VALUE style)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    if ( style == sym_inline )
    {
        node->data.pairs->style = map_inline;
    }
    else
    {
        node->data.pairs->style = map_none;
    }

    rb_iv_set( self, "@style", style );
    return self;
}

#if 0
/*
 * Cloning method for all node types
 */
VALUE
syck_node_init_copy(VALUE copy, VALUE orig)
{
    SyckNode *copy_n;
    SyckNode *orig_n;

    if ( copy == orig )
        return copy;

    if ( TYPE( orig ) != T_DATA )
    {
        rb_raise( rb_eTypeError, "wrong argument type" );
    }

    Data_Get_Struct( orig, SyckNode, orig_n );
    Data_Get_Struct( copy, SyckNode, copy_n );
    MEMCPY( copy_n, orig_n, SyckNode, 1 );
    return copy;
}
#endif

/*
 * YAML::Syck::Node#type_id=
 */
VALUE
syck_node_type_id_set(VALUE self, VALUE type_id)
{
    SyckNode *node;
    Data_Get_Struct( self, SyckNode, node );

    S_FREE( node->type_id );

    if ( !NIL_P( type_id ) ) {
        StringValue( type_id );
        node->type_id = syck_strndup( RSTRING_PTR(type_id), RSTRING_LEN(type_id) );
    }

    rb_iv_set( self, "@type_id", type_id );
    return type_id;
}

/*
 * YAML::Syck::Node.transform
 */
VALUE
syck_node_transform(VALUE self)
{
    VALUE t;
    SyckNode *n = NULL;
    SyckNode *orig_n;
    Data_Get_Struct(self, SyckNode, orig_n);
    t = Data_Wrap_Struct( cNode, syck_node_mark, syck_free_node, 0 );

    switch (orig_n->kind)
    {
        case syck_map_kind:
            {
                int i;
                DATA_PTR(t) = n = syck_alloc_map();
                for ( i = 0; i < orig_n->data.pairs->idx; i++ )
                {
                    syck_map_add( n, rb_funcall( syck_map_read( orig_n, map_key, i ), s_transform, 0 ),
                                     rb_funcall( syck_map_read( orig_n, map_value, i ), s_transform, 0 ) );
                }
            }
        break;

        case syck_seq_kind:
            {
                int i;
                DATA_PTR(t) = n = syck_alloc_seq();
                for ( i = 0; i < orig_n->data.list->idx; i++ )
                {
                    syck_seq_add( n, rb_funcall( syck_seq_read( orig_n, i ), s_transform, 0 ) );
                }
            }
        break;

        case syck_str_kind:
            DATA_PTR(t) = n = syck_new_str2( orig_n->data.str->ptr, orig_n->data.str->len, orig_n->data.str->style );
        break;
    }

    if ( orig_n->type_id != NULL )
    {
        n->type_id = syck_strndup( orig_n->type_id, strlen( orig_n->type_id ) );
    }
    if ( orig_n->anchor != NULL )
    {
        n->anchor = syck_strndup( orig_n->anchor, strlen( orig_n->anchor ) );
    }
    n->id = t;
    return rb_funcall( oDefaultResolver, s_node_import, 1, t );
}

/*
 * Emitter callback: assembles YAML document events from
 * Ruby symbols.  This is a brilliant way to do it.
 * No one could possibly object.
 */
void
rb_syck_emitter_handler(SyckEmitter *e, st_data_t data)
{
    SyckNode *n;
    Data_Get_Struct((VALUE)data, SyckNode, n);

    switch (n->kind)
    {
        case syck_map_kind:
            {
                int i;
                syck_emit_map( e, n->type_id, n->data.pairs->style );
                for ( i = 0; i < n->data.pairs->idx; i++ )
                {
                    syck_emit_item( e, syck_map_read( n, map_key, i ) );
                    syck_emit_item( e, syck_map_read( n, map_value, i ) );
                }
                syck_emit_end( e );
            }
        break;

        case syck_seq_kind:
            {
                int i;
                syck_emit_seq( e, n->type_id, n->data.list->style );
                for ( i = 0; i < n->data.list->idx; i++ )
                {
                    syck_emit_item( e, syck_seq_read( n, i ) );
                }
                syck_emit_end( e );
            }
        break;

        case syck_str_kind:
            {
                syck_emit_scalar( e, n->type_id, n->data.str->style, 0, 0, 0, n->data.str->ptr, n->data.str->len );
            }
        break;
    }
}

/*
 * Handle output from the emitter
 */
void
rb_syck_output_handler(SyckEmitter * emitter, char *str, long len)
{
    struct emitter_xtra *bonus = (struct emitter_xtra *)emitter->bonus;
    VALUE dest = bonus->port;
    if (TYPE(dest) == T_STRING) {
        rb_str_cat( dest, str, len );
    } else {
        rb_io_write( dest, rb_str_new( str, len ) );
    }
}

/*
 * Helper function for marking nodes in the anchor
 * symbol table.
 */
void
syck_out_mark(VALUE emitter, VALUE node)
{
    SyckEmitter *emitterPtr;
    struct emitter_xtra *bonus;
    Data_Get_Struct(emitter, SyckEmitter, emitterPtr);
    bonus = (struct emitter_xtra *)emitterPtr->bonus;
    rb_ivar_set( node, s_emitter, emitter );
    /* syck_emitter_mark_node( emitterPtr, (st_data_t)node ); */
    if ( !NIL_P( bonus->oid ) ) {
        rb_hash_aset( bonus->data, bonus->oid, node );
    }
}

/*
 * Mark emitter values.
 */
static void
syck_mark_emitter(SyckEmitter *emitter)
{
    struct emitter_xtra *bonus = (struct emitter_xtra *)emitter->bonus;
    rb_gc_mark( bonus->oid  );
    rb_gc_mark( bonus->data );
    rb_gc_mark( bonus->port );
}

/*
 * Free the emitter and any bonus attachment.
 */
void
rb_syck_free_emitter(SyckEmitter *e)
{
    S_FREE( e->bonus );
    syck_free_emitter(e);
}

/*
 * YAML::Syck::Emitter.allocate
 */
VALUE syck_emitter_s_alloc _((VALUE));
VALUE
syck_emitter_s_alloc(VALUE class)
{
    VALUE pobj;
    SyckEmitter *emitter = syck_new_emitter();

    emitter->bonus = S_ALLOC( struct emitter_xtra );
    S_MEMZERO( emitter->bonus, struct emitter_xtra, 1 );

    pobj = Data_Wrap_Struct( class, syck_mark_emitter, rb_syck_free_emitter, emitter );
    syck_emitter_handler( emitter, rb_syck_emitter_handler );
    syck_output_handler( emitter, rb_syck_output_handler );

    rb_ivar_set( pobj, s_out, rb_funcall( cOut, s_new, 1, pobj ) );
    return pobj;
}

static VALUE
id_hash_new(void)
{
    VALUE hash;
    hash = rb_hash_new();
    rb_funcall(hash, rb_intern("compare_by_identity"), 0);
    return hash;
}

/*
 * YAML::Syck::Emitter.reset( options )
 */
VALUE
syck_emitter_reset(int argc, VALUE *argv, VALUE self)
{
    VALUE options, tmp;
    SyckEmitter *emitter;
    struct emitter_xtra *bonus;

    Data_Get_Struct(self, SyckEmitter, emitter);
    bonus = (struct emitter_xtra *)emitter->bonus;

    bonus->oid = Qnil;
    bonus->port = rb_str_new2( "" );
    bonus->data = id_hash_new();

    if (rb_scan_args(argc, argv, "01", &options) == 0)
    {
        options = rb_hash_new();
        rb_ivar_set(self, s_options, options);
    }
    else if ( !NIL_P(tmp = rb_check_string_type(options)) )
    {
        bonus->port = tmp;
    }
    else if ( rb_respond_to( options, s_write ) )
    {
        bonus->port = options;
    }
    else
    {
        Check_Type(options, T_HASH);
        rb_ivar_set(self, s_options, options);
    }

    emitter->headless = 0;
    rb_ivar_set(self, s_level, INT2FIX(0));
    rb_ivar_set(self, s_resolver, Qnil);
    return self;
}

/*
 * YAML::Syck::Emitter.emit( object_id ) { |out| ... }
 */
VALUE
syck_emitter_emit(int argc, VALUE *argv, VALUE self)
{
    VALUE oid, proc;
    SyckEmitter *emitter;
    struct emitter_xtra *bonus;
    SYMID symple;
    int level = FIX2INT(rb_ivar_get(self, s_level)) + 1;
    rb_ivar_set(self, s_level, INT2FIX(level));

    rb_scan_args(argc, argv, "1&", &oid, &proc);
    Data_Get_Struct(self, SyckEmitter, emitter);
    bonus = (struct emitter_xtra *)emitter->bonus;

    /* Calculate anchors, normalize nodes, build a simpler symbol table */
    bonus->oid = oid;
    if ( !NIL_P( oid ) && RTEST( rb_funcall( bonus->data, s_haskey, 1, oid ) ) ) {
        symple = rb_hash_aref( bonus->data, oid );
    } else {
        symple = rb_funcall( proc, s_call, 1, rb_ivar_get( self, s_out ) );
    }
    syck_emitter_mark_node( emitter, (st_data_t)symple );

    /* Second pass, build emitted string */
    level -= 1;
    rb_ivar_set(self, s_level, INT2FIX(level));
    if ( level == 0 )
    {
        syck_emit(emitter, (st_data_t)symple);
        syck_emitter_flush(emitter, 0);

        return bonus->port;
    }

    return symple;
}

/*
 * YAML::Syck::Emitter#node_export
 */
VALUE
syck_emitter_node_export(VALUE self, VALUE node)
{
    return rb_funcall( node, s_to_yaml, 1, self );
}

/*
 * YAML::Syck::Emitter#set_resolver
 */
VALUE
syck_emitter_set_resolver(VALUE self, VALUE resolver)
{
    rb_ivar_set( self, s_resolver, resolver );
    return self;
}

/*
 * YAML::Syck::Out::initialize
 */
VALUE
syck_out_initialize(VALUE self, VALUE emitter)
{
    rb_ivar_set( self, s_emitter, emitter );
    return self;
}

/*
 * YAML::Syck::Out::map
 */
VALUE
syck_out_map(int argc, VALUE *argv, VALUE self)
{
    VALUE type_id, style, map;
    if (rb_scan_args(argc, argv, "11", &type_id, &style) == 1) {
        style = Qnil;
    }
    map = rb_funcall( cMap, s_new, 3, type_id, rb_hash_new(), style );
    syck_out_mark( rb_ivar_get( self, s_emitter ), map );
    rb_yield( map );
    return map;
}

/*
 * YAML::Syck::Out::seq
 */
VALUE
syck_out_seq(int argc, VALUE *argv, VALUE self)
{
    VALUE type_id, style, seq;
    if (rb_scan_args(argc, argv, "11", &type_id, &style) == 1) {
        style = Qnil;
    }
    seq = rb_funcall( cSeq, s_new, 3, type_id, rb_ary_new(), style );
    syck_out_mark( rb_ivar_get( self, s_emitter ), seq );
    rb_yield( seq );
    return seq;
}

/*
 * YAML::Syck::Out::scalar
syck_out_scalar( self, type_id, str, style )
    VALUE self, type_id, str, style;
 */
VALUE
syck_out_scalar(int argc, VALUE *argv, VALUE self)
{
    VALUE type_id, str, style, scalar;
    rb_scan_args(argc, argv, "21", &type_id, &str, &style);
    scalar = rb_funcall( cScalar, s_new, 3, type_id, str, style );
    syck_out_mark( rb_ivar_get( self, s_emitter ), scalar );
    return scalar;
}

/*
 * Initialize Syck extension
 */
void
Init_syck()
{
    VALUE rb_syck = rb_define_module_under( rb_cObject, "Syck" );
    rb_define_module_function( rb_syck, "compile", rb_syck_compile, 1 );

    /*
     * Global symbols
     */
    s_new = rb_intern("new");
    s_utc = rb_intern("utc");
    s_at = rb_intern("at");
    s_to_f = rb_intern("to_f");
    s_to_i = rb_intern("to_i");
    s_read = rb_intern("read");
    s_binmode = rb_intern("binmode");
    s_transfer = rb_intern("transfer");
    s_call = rb_intern("call");
    s_cmp = rb_intern("<=>");
    s_intern = rb_intern("intern");
    s_update = rb_intern("update");
    s_detect_implicit = rb_intern("detect_implicit");
    s_dup = rb_intern("dup");
    s_default_set = rb_intern("default=");
    s_match = rb_intern("match");
    s_push = rb_intern("push");
    s_haskey = rb_intern("has_key?");
    s_keys = rb_intern("keys");
    s_node_import = rb_intern("node_import");
    s_tr_bang = rb_intern("tr!");
    s_unpack = rb_intern("unpack");
    s_write = rb_intern("write");
    s_tag_read_class = rb_intern( "yaml_tag_read_class" );
    s_tag_subclasses = rb_intern( "yaml_tag_subclasses?" );
    s_emitter = rb_intern( "emitter" );
    s_set_resolver = rb_intern( "set_resolver" );
    s_node_export = rb_intern( "node_export" );
    s_to_yaml = rb_intern( "to_yaml" );
    s_transform = rb_intern( "transform" );
    s_yaml_new = rb_intern("yaml_new");
    s_yaml_initialize = rb_intern("yaml_initialize");
    s_each = rb_intern("each");
    s_parse = rb_intern("parse");

    s_tags = rb_intern("@tags");
    s_name = rb_intern("@name");
    s_options = rb_intern("@options");
    s_kind = rb_intern("@kind");
    s_type_id = rb_intern("@type_id");
    s_type_id_set = rb_intern("type_id=");
    s_resolver = rb_intern("@resolver");
    s_level = rb_intern( "@level" );
    s_style = rb_intern("@style");
    s_style_set = rb_intern("style=");
    s_value = rb_intern("@value");
    s_value_set = rb_intern("value=");
    s_out = rb_intern("@out");
    s_input = rb_intern("@input");

    sym_model = ID2SYM(rb_intern("Model"));
    sym_generic = ID2SYM(rb_intern("Generic"));
    sym_bytecode = ID2SYM(rb_intern("bytecode"));
    sym_map = ID2SYM(rb_intern("map"));
    sym_scalar = ID2SYM(rb_intern("scalar"));
    sym_seq = ID2SYM(rb_intern("seq"));
    sym_1quote = ID2SYM(rb_intern("quote1"));
    sym_2quote = ID2SYM(rb_intern("quote2"));
    sym_fold = ID2SYM(rb_intern("fold"));
    sym_literal = ID2SYM(rb_intern("literal"));
    sym_plain = ID2SYM(rb_intern("plain"));
    sym_inline = ID2SYM(rb_intern("inline"));

    /*
     * Define YAML::Syck::Resolver class
     */
    cResolver = rb_define_class_under( rb_syck, "Resolver", rb_cObject );
    rb_define_attr( cResolver, "tags", 1, 1 );
    rb_define_method( cResolver, "initialize", syck_resolver_initialize, 0 );
    rb_define_method( cResolver, "add_type", syck_resolver_add_type, 2 );
    rb_define_method( cResolver, "use_types_at", syck_resolver_use_types_at, 1 );
    rb_define_method( cResolver, "detect_implicit", syck_resolver_detect_implicit, 1 );
    rb_define_method( cResolver, "transfer", syck_resolver_transfer, 2 );
    rb_define_method( cResolver, "node_import", syck_resolver_node_import, 1 );
    rb_define_method( cResolver, "tagurize", syck_resolver_tagurize, 1 );

    rb_global_variable( &oDefaultResolver );
    oDefaultResolver = rb_funcall( cResolver, rb_intern( "new" ), 0 );
    rb_define_singleton_method( oDefaultResolver, "node_import", syck_defaultresolver_node_import, 1 );
    rb_define_singleton_method( oDefaultResolver, "detect_implicit", syck_defaultresolver_detect_implicit, 1 );
    rb_define_const( rb_syck, "DefaultResolver", oDefaultResolver );
    rb_global_variable( &oGenericResolver );
    oGenericResolver = rb_funcall( cResolver, rb_intern( "new" ), 0 );
    rb_define_singleton_method( oGenericResolver, "node_import", syck_genericresolver_node_import, 1 );
    rb_define_const( rb_syck, "GenericResolver", oGenericResolver );

    /*
     * Define YAML::Syck::Parser class
     */
    cParser = rb_define_class_under( rb_syck, "Parser", rb_cObject );
    rb_define_attr( cParser, "options", 1, 1 );
    rb_define_attr( cParser, "resolver", 1, 1 );
    rb_define_attr( cParser, "input", 1, 1 );
    rb_define_alloc_func( cParser, syck_parser_s_alloc );
    rb_define_method(cParser, "initialize", syck_parser_initialize, -1 );
    rb_define_method(cParser, "bufsize=", syck_parser_bufsize_set, 1 );
    rb_define_method(cParser, "bufsize", syck_parser_bufsize_get, 0 );
    rb_define_method(cParser, "load", syck_parser_load, -1);
    rb_define_method(cParser, "load_documents", syck_parser_load_documents, -1);
    rb_define_method(cParser, "set_resolver", syck_parser_set_resolver, 1);

    /*
     * Define YAML::Syck::Node class
     */
    cNode = rb_define_class_under( rb_syck, "Node", rb_cObject );
    rb_undef( cNode, rb_intern("initialize_copy") );
    rb_define_attr( cNode, "emitter", 1, 1 );
    rb_define_attr( cNode, "resolver", 1, 1 );
    rb_define_attr( cNode, "kind", 1, 0 );
    rb_define_attr( cNode, "type_id", 1, 0 );
    rb_define_attr( cNode, "value", 1, 0 );
    rb_define_method( cNode, "type_id=", syck_node_type_id_set, 1 );
    rb_define_method( cNode, "transform", syck_node_transform, 0);

    /*
     * Define YAML::Syck::Scalar, YAML::Syck::Seq, YAML::Syck::Map --
     *     all are the publicly usable variants of YAML::Syck::Node
     */
    cScalar = rb_define_class_under( rb_syck, "Scalar", cNode );
    rb_define_alloc_func( cScalar, syck_scalar_alloc );
    rb_define_method( cScalar, "initialize", syck_scalar_initialize, 3 );
    rb_define_method( cScalar, "value=", syck_scalar_value_set, 1 );
    rb_define_method( cScalar, "style=", syck_scalar_style_set, 1 );
    cSeq = rb_define_class_under( rb_syck, "Seq", cNode );
    rb_define_alloc_func( cSeq, syck_seq_alloc );
    rb_define_method( cSeq, "initialize", syck_seq_initialize, 3 );
    rb_define_method( cSeq, "value=", syck_seq_value_set, 1 );
    rb_define_method( cSeq, "add", syck_seq_add_m, 1 );
    rb_define_method( cSeq, "style=", syck_seq_style_set, 1 );
    cMap = rb_define_class_under( rb_syck, "Map", cNode );
    rb_define_alloc_func( cMap, syck_map_alloc );
    rb_define_method( cMap, "initialize", syck_map_initialize, 3 );
    rb_define_method( cMap, "value=", syck_map_value_set, 1 );
    rb_define_method( cMap, "add", syck_map_add_m, 2 );
    rb_define_method( cMap, "style=", syck_map_style_set, 1 );

    /*
     * Define YAML::PrivateType class
     */
    cPrivateType = rb_define_class_under( rb_syck, "PrivateType", rb_cObject );
    rb_define_attr( cPrivateType, "type_id", 1, 1 );
    rb_define_attr( cPrivateType, "value", 1, 1 );
    rb_define_method( cPrivateType, "initialize", syck_privatetype_initialize, 2);

    /*
     * Define YAML::DomainType class
     */
    cDomainType = rb_define_class_under( rb_syck, "DomainType", rb_cObject );
    rb_define_attr( cDomainType, "domain", 1, 1 );
    rb_define_attr( cDomainType, "type_id", 1, 1 );
    rb_define_attr( cDomainType, "value", 1, 1 );
    rb_define_method( cDomainType, "initialize", syck_domaintype_initialize, 3);

    /*
     * Define YAML::Object class
     */
    cYObject = rb_define_class_under( rb_syck, "Object", rb_cObject );
    rb_define_attr( cYObject, "class", 1, 1 );
    rb_define_attr( cYObject, "ivars", 1, 1 );
    rb_define_method( cYObject, "initialize", syck_yobject_initialize, 2);
    rb_define_method( cYObject, "yaml_initialize", syck_yobject_initialize, 2);

    /*
     * Define YAML::Syck::BadAlias class
     */
    cBadAlias = rb_define_class_under( rb_syck, "BadAlias", rb_cObject );
    rb_define_attr( cBadAlias, "name", 1, 1 );
    rb_define_method( cBadAlias, "initialize", syck_badalias_initialize, 1);
    rb_define_method( cBadAlias, "<=>", syck_badalias_cmp, 1);
    rb_include_module( cBadAlias, rb_const_get( rb_cObject, rb_intern("Comparable") ) );

    /*
     * Define YAML::Syck::MergeKey class
     */
    cMergeKey = rb_define_class_under( rb_syck, "MergeKey", rb_cObject );

    /*
     * Define YAML::Syck::DefaultKey class
     */
    cDefaultKey = rb_define_class_under( rb_syck, "DefaultKey", rb_cObject );

    /*
     * Define YAML::Syck::Out classes
     */
    cOut = rb_define_class_under( rb_syck, "Out", rb_cObject );
    rb_define_attr( cOut, "emitter", 1, 1 );
    rb_define_method( cOut, "initialize", syck_out_initialize, 1 );
    rb_define_method( cOut, "map", syck_out_map, -1 );
    rb_define_method( cOut, "seq", syck_out_seq, -1 );
    rb_define_method( cOut, "scalar", syck_out_scalar, -1 );

    /*
     * Define YAML::Syck::Emitter class
     */
    cEmitter = rb_define_class_under( rb_syck, "Emitter", rb_cObject );
    rb_define_attr( cEmitter, "level", 1, 1 );
    rb_define_alloc_func( cEmitter, syck_emitter_s_alloc );
    rb_define_method( cEmitter, "initialize", syck_emitter_reset, -1 );
    rb_define_method( cEmitter, "reset", syck_emitter_reset, -1 );
    rb_define_method( cEmitter, "emit", syck_emitter_emit, -1 );
    rb_define_method( cEmitter, "set_resolver", syck_emitter_set_resolver, 1);
    rb_define_method( cEmitter, "node_export", syck_emitter_node_export, 1);
}

