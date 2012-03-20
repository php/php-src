/*
 * yaml2byte.c
 *
 * $Author$
 *
 * Copyright (C) 2003 why the lucky stiff, clark evans
 *
 *   WARNING WARNING WARNING  --- THIS IS *NOT JUST* PLAYING
 *   ANYMORE! -- WHY HAS EMBRACED THIS AS THE REAL THING!
 */
#include "ruby/ruby.h"
#include <syck.h>
#include <assert.h>
#define YAMLBYTE_UTF8
#include "yamlbyte.h"

#include <stdio.h>
#define TRACE0(a)  \
    do { printf(a); printf("\n"); fflush(stdout); } while(0)
#define TRACE1(a,b) \
    do { printf(a,b); printf("\n"); fflush(stdout); } while(0)
#define TRACE2(a,b,c) \
    do { printf(a,b,c); printf("\n"); fflush(stdout); } while(0)
#define TRACE3(a,b,c,d) \
    do { printf(a,b,c,d); printf("\n"); fflush(stdout); } while(0)

/* Reinvent the wheel... */
#define CHUNKSIZE 64
#define HASH ((long)0xCAFECAFE)
typedef struct {
   long hash;
   char *buffer;
   long length;
   long remaining;
   int  printed;
} bytestring_t;
bytestring_t *bytestring_alloc(void) {
    bytestring_t *ret;
    /*TRACE0("bytestring_alloc()");*/
    ret = S_ALLOC(bytestring_t);
    ret->hash   = HASH;
    ret->length = CHUNKSIZE;
    ret->remaining = ret->length;
    ret->buffer = S_ALLOC_N(char, ret->length + 1 );
    ret->buffer[0] = 0;
    ret->printed = 0;
    return ret;
}
void bytestring_append(bytestring_t *str, char code,
                       char *start, char *finish)
{
    long grow;
    long length = 2;   /* CODE + LF */
    char *curr;
    assert(str && HASH == str->hash);
    /*TRACE0("bytestring_append()");*/
    if(start) {
        if(!finish)
            finish = start + strlen(start);
        length += (finish-start);
    }
    if(length > str->remaining) {
        grow = (length - str->remaining) + CHUNKSIZE;
        str->remaining += grow;
        str->length    += grow;
        S_REALLOC_N( str->buffer, char, str->length + 1 );
        assert(str->buffer);
    }
    curr = str->buffer + (str->length - str->remaining);
    *curr = code;
    curr += 1;
    if(start)
        while(start < finish)
            *curr ++ = *start ++;
    *curr = '\n';
    curr += 1;
    *curr = 0;
    str->remaining = str->remaining - length;
    assert( (str->buffer + str->length) - str->remaining );
}
void bytestring_extend(bytestring_t *str, bytestring_t *ext)
{
    char *from;
    char *curr;
    char *stop;
    long grow;
    long length;
    assert(str && HASH == str->hash);
    assert(ext && HASH == ext->hash);
    if(ext->printed) {
        assert(ext->buffer[0] ==YAMLBYTE_ANCHOR);
        curr = ext->buffer;
        while( '\n' != *curr)
            curr++;
        bytestring_append(str, YAMLBYTE_ALIAS, ext->buffer + 1, curr);
    } else {
        ext->printed = 1;
        length  = (ext->length - ext->remaining);
        if(length > str->remaining) {
            grow = (length - str->remaining) + CHUNKSIZE;
            str->remaining += grow;
            str->length    += grow;
            S_REALLOC_N( str->buffer, char, str->length + 1 );
        }
        curr = str->buffer + (str->length - str->remaining);
        from = ext->buffer;
        stop = ext->buffer + length;
        while( from < stop )
            *curr ++ = *from ++;
        *curr = 0;
        str->remaining = str->remaining - length;
        assert( (str->buffer + str->length) - str->remaining );
    }
}

/* convert SyckNode into yamlbyte_buffer_t objects */
SYMID
syck_yaml2byte_handler(p, n)
    SyckParser *p;
    SyckNode *n;
{
    SYMID oid;
    long i;
    char ch;
    char nextcode;
    char *start;
    char *current;
    char *finish;
    bytestring_t *val = NULL;
    bytestring_t *sav = NULL;
    void *data;
    /*TRACE0("syck_yaml2byte_handler()");*/
    val = bytestring_alloc();
    if(n->anchor) bytestring_append(val,YAMLBYTE_ANCHOR, n->anchor, NULL);
    if ( n->type_id )
    {
        if ( p->taguri_expansion )
        {
            bytestring_append(val,YAMLBYTE_TRANSFER, n->type_id, NULL);
        }
        else
        {
            char *type_tag = S_ALLOC_N( char, strlen( n->type_id ) + 1 );
            type_tag[0] = '\0';
            strcat( type_tag, "!" );
            strcat( type_tag, n->type_id );
            bytestring_append( val, YAMLBYTE_TRANSFER, type_tag, NULL);
	    S_FREE(type_tag);
        }
    }
    switch (n->kind)
    {
        case syck_str_kind:
            nextcode = YAMLBYTE_SCALAR;
            start  = n->data.str->ptr;
            finish = start + n->data.str->len - 1;
            current = start;
            /*TRACE2("SCALAR: %s %d", start, n->data.str->len); */
            while(1) {
                ch = *current;
                if('\n' == ch || 0 == ch || current > finish) {
                    if(current >= start) {
                        bytestring_append(val, nextcode, start, current);
                        nextcode = YAMLBYTE_CONTINUE;
                    }
                    start = current + 1;
                    if(current > finish)
                    {
                        break;
                    }
                    else if('\n' == ch )
                    {
                        bytestring_append(val,YAMLBYTE_NEWLINE,NULL,NULL);
                    }
                    else if(0 == ch)
                    {
                        bytestring_append(val,YAMLBYTE_NULLCHAR,NULL,NULL);
                    }
                    else
                    {
                        assert("oops");
                    }
                }
                current += 1;
            }
        break;
        case syck_seq_kind:
            bytestring_append(val,YAMLBYTE_SEQUENCE,NULL,NULL);
            for ( i = 0; i < n->data.list->idx; i++ )
            {
                oid = syck_seq_read( n, i );
                if (syck_lookup_sym( p, oid, &data )) sav = data;
                bytestring_extend(val, sav);
            }
            bytestring_append(val,YAMLBYTE_END_BRANCH,NULL,NULL);
        break;
        case syck_map_kind:
            bytestring_append(val,YAMLBYTE_MAPPING,NULL,NULL);
            for ( i = 0; i < n->data.pairs->idx; i++ )
            {
                oid = syck_map_read( n, map_key, i );
                if (syck_lookup_sym( p, oid, &data )) sav = data;
                bytestring_extend(val, sav);
                oid = syck_map_read( n, map_value, i );
                if (syck_lookup_sym( p, oid, &data )) sav = data;
                bytestring_extend(val, sav);
            }
            bytestring_append(val,YAMLBYTE_END_BRANCH,NULL,NULL);
        break;
    }
    oid = syck_add_sym( p, (char *) val );
    /*TRACE1("Saving: %s", val->buffer );*/
    return oid;
}

char *
syck_yaml2byte(char *yamlstr)
{
    SYMID oid;
    char *ret;
    bytestring_t *sav;
    void *data;

    SyckParser *parser = syck_new_parser();
    syck_parser_str_auto( parser, yamlstr, NULL );
    syck_parser_handler( parser, syck_yaml2byte_handler );
    syck_parser_error_handler( parser, NULL );
    syck_parser_implicit_typing( parser, 1 );
    syck_parser_taguri_expansion( parser, 1 );
    oid = syck_parse( parser );

    if ( syck_lookup_sym( parser, oid, &data ) ) {
	sav = data;
        ret = S_ALLOC_N( char, strlen( sav->buffer ) + 3 );
        ret[0] = '\0';
        strcat( ret, "D\n" );
        strcat( ret, sav->buffer );
    }
    else
    {
        ret = NULL;
    }

    syck_free_parser( parser );
    return ret;
}

#ifdef TEST_YBEXT
#include <stdio.h>
int main() {
   char *yaml = "test: 1\nand: \"with new\\nline\\n\"\nalso: &3 three\nmore: *3";
   printf("--- # YAML \n");
   printf(yaml);
   printf("\n...\n");
   printf(syck_yaml2byte(yaml));
   return 0;
}
#endif

