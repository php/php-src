/*
 * syck.c
 *
 * $Author$
 *
 * Copyright (C) 2003 why the lucky stiff
 */
#include "ruby/ruby.h"

#include <stdio.h>
#include <string.h>

#include "syck.h"

void syck_parser_pop_level( SyckParser * );

/*
 * Custom assert
 */
void
syck_assert( const char *file_name, unsigned line_num, const char *expr )
{
    fflush( NULL );
    fprintf( stderr, "\nAssertion failed: %s, line %u: %s\n",
             file_name, line_num, expr );
    fflush( stderr );
    abort();
}

/*
 * Allocates and copies a string
 */
char *
syck_strndup( const char *buf, long len )
{
    char *new = S_ALLOC_N( char, len + 1 );
    S_MEMZERO( new, char, len + 1 );
    S_MEMCPY( new, buf, char, len );
    return new;
}

/*
 * Default FILE IO function
 */
long
syck_io_file_read( char *buf, SyckIoFile *file, long max_size, long skip )
{
    long len = 0;

    ASSERT( file != NULL );

    max_size -= skip;
    len = fread( buf + skip, sizeof( char ), max_size, file->ptr );
    len += skip;
    buf[len] = '\0';

    return len;
}

/*
 * Default string IO function
 */
long
syck_io_str_read( char *buf, SyckIoStr *str, long max_size, long skip )
{
    char *beg;
    long len = 0;

    ASSERT( str != NULL );
    beg = str->ptr;
    if ( max_size >= 0 )
    {
        max_size -= skip;
        if ( max_size <= 0 )  max_size = 0;
        else                  str->ptr += max_size;

        if ( str->ptr > str->end )
        {
            str->ptr = str->end;
        }
    }
    else
    {
        /* Use exact string length */
        while ( str->ptr < str->end ) {
            if (*(str->ptr++) == '\n') break;
        }
    }
    if ( beg < str->ptr )
    {
        len = ( str->ptr - beg );
        S_MEMCPY( buf + skip, beg, char, len );
    }
    len += skip;
    buf[len] = '\0';

    return len;
}

void
syck_parser_reset_levels( SyckParser *p )
{
    while ( p->lvl_idx > 1 )
    {
        syck_parser_pop_level( p );
    }

    if ( p->lvl_idx < 1 )
    {
        p->lvl_idx = 1;
        p->levels[0].spaces = -1;
        p->levels[0].ncount = 0;
        p->levels[0].domain = syck_strndup( "", 0 );
    }
    p->levels[0].status = syck_lvl_header;
}

void
syck_parser_reset_cursor( SyckParser *p )
{
    if ( p->buffer == NULL )
    {
        p->buffer = S_ALLOC_N( char, p->bufsize );
        S_MEMZERO( p->buffer, char, p->bufsize );
    }
    p->buffer[0] = '\0';

    p->cursor = NULL;
    p->lineptr = NULL;
    p->linectptr = NULL;
    p->token = NULL;
    p->toktmp = NULL;
    p->marker = NULL;
    p->limit = NULL;

    p->root = 0;
    p->root_on_error = 0;
    p->linect = 0;
    p->eof = 0;
    p->last_token = 0;
    p->force_token = 0;
}

/*
 * Value to return on a parse error
 */
void
syck_parser_set_root_on_error( SyckParser *p, SYMID roer )
{
    p->root_on_error = roer;
}

/*
 * Allocate the parser
 */
SyckParser *
syck_new_parser(void)
{
    SyckParser *p;
    p = S_ALLOC( SyckParser );
    S_MEMZERO( p, SyckParser, 1 );
    p->lvl_capa = ALLOC_CT;
    p->levels = S_ALLOC_N( SyckLevel, p->lvl_capa );
    p->input_type = syck_yaml_utf8;
    p->io_type = syck_io_str;
    p->io.str = NULL;
    p->syms = NULL;
    p->anchors = NULL;
    p->bad_anchors = NULL;
    p->implicit_typing = 1;
    p->taguri_expansion = 0;
    p->bufsize = SYCK_BUFFERSIZE;
    p->buffer = NULL;
    p->lvl_idx = 0;
    syck_parser_reset_levels( p );
    return p;
}

int
syck_add_sym( SyckParser *p, void *data )
{
    SYMID id = 0;
    if ( p->syms == NULL )
    {
        p->syms = st_init_numtable();
    }
    id = p->syms->num_entries + 1;
    st_insert( p->syms, id, (st_data_t)data );
    return (int)id;
}

int
syck_lookup_sym( SyckParser *p, SYMID id, void **datap )
{
    st_data_t data;
    int ret;
    if ( p->syms == NULL ) return 0;
    ret = st_lookup( p->syms, id, &data );
    if(ret) *datap = (void *)data;
    return ret;
}

int
syck_st_free_nodes( char *key, SyckNode *n, char *arg )
{
    if ( n != (void *)1 ) syck_free_node( n );
    n = NULL;
    return ST_CONTINUE;
}

void
syck_st_free( SyckParser *p )
{
    /*
     * Free the anchor tables
     */
    if ( p->anchors != NULL )
    {
        st_foreach( p->anchors, syck_st_free_nodes, 0 );
        st_free_table( p->anchors );
        p->anchors = NULL;
    }

    if ( p->bad_anchors != NULL )
    {
        st_foreach( p->bad_anchors, syck_st_free_nodes, 0 );
        st_free_table( p->bad_anchors );
        p->bad_anchors = NULL;
    }
}

typedef struct {
   long hash;
   char *buffer;
   long length;
   long remaining;
   int  printed;
} bytestring_t;

int
syck_st_free_syms( void *key, bytestring_t *sav, void *dummy )
{
    S_FREE(sav->buffer);
    S_FREE(sav);
    return ST_CONTINUE;
}

void
syck_free_parser( SyckParser *p )
{
    /*
     * Free the adhoc symbol table
     */
    if ( p->syms != NULL )
    {
        st_foreach( p->syms, syck_st_free_syms, 0 );
        st_free_table( p->syms );
        p->syms = NULL;
    }

    /*
     * Free tables, levels
     */
    syck_st_free( p );
    syck_parser_reset_levels( p );
    S_FREE( p->levels[0].domain );
    S_FREE( p->levels );

    if ( p->buffer != NULL )
    {
        S_FREE( p->buffer );
    }
    free_any_io( p );
    S_FREE( p );
}

void
syck_parser_handler( SyckParser *p, SyckNodeHandler hdlr )
{
    ASSERT( p != NULL );
    p->handler = hdlr;
}

void
syck_parser_implicit_typing( SyckParser *p, int flag )
{
    p->implicit_typing = ( flag == 0 ? 0 : 1 );
}

void
syck_parser_taguri_expansion( SyckParser *p, int flag )
{
    p->taguri_expansion = ( flag == 0 ? 0 : 1 );
}

void
syck_parser_error_handler( SyckParser *p, SyckErrorHandler hdlr )
{
    ASSERT( p != NULL );
    p->error_handler = hdlr;
}

void
syck_parser_bad_anchor_handler( SyckParser *p, SyckBadAnchorHandler hdlr )
{
    ASSERT( p != NULL );
    p->bad_anchor_handler = hdlr;
}

void
syck_parser_set_input_type( SyckParser *p, enum syck_parser_input input_type )
{
    ASSERT( p != NULL );
    p->input_type = input_type;
}

void
syck_parser_file( SyckParser *p, FILE *fp, SyckIoFileRead read )
{
    ASSERT( p != NULL );
    free_any_io( p );
	syck_parser_reset_cursor( p );
    p->io_type = syck_io_file;
    p->io.file = S_ALLOC( SyckIoFile );
    p->io.file->ptr = fp;
    if ( read != NULL )
    {
        p->io.file->read = read;
    }
    else
    {
        p->io.file->read = syck_io_file_read;
    }
}

void
syck_parser_str( SyckParser *p, char *ptr, long len, SyckIoStrRead read )
{
    ASSERT( p != NULL );
    free_any_io( p );
	syck_parser_reset_cursor( p );
    p->io_type = syck_io_str;
    p->io.str = S_ALLOC( SyckIoStr );
    p->io.str->beg = ptr;
    p->io.str->ptr = ptr;
    p->io.str->end = ptr + len;
    if ( read != NULL )
    {
        p->io.str->read = read;
    }
    else
    {
        p->io.str->read = syck_io_str_read;
    }
}

void
syck_parser_str_auto( SyckParser *p, char *ptr, SyckIoStrRead read )
{
    syck_parser_str( p, ptr, strlen( ptr ), read );
}

SyckLevel *
syck_parser_current_level( SyckParser *p )
{
    return &p->levels[p->lvl_idx-1];
}

void
syck_parser_pop_level( SyckParser *p )
{
    ASSERT( p != NULL );

    /* The root level should never be popped */
    if ( p->lvl_idx <= 1 ) return;

    p->lvl_idx -= 1;
    free( p->levels[p->lvl_idx].domain );
}

void
syck_parser_add_level( SyckParser *p, int len, enum syck_level_status status )
{
    ASSERT( p != NULL );
    if ( p->lvl_idx + 1 > p->lvl_capa )
    {
        p->lvl_capa += ALLOC_CT;
        S_REALLOC_N( p->levels, SyckLevel, p->lvl_capa );
    }

    ASSERT( len > p->levels[p->lvl_idx-1].spaces );
    p->levels[p->lvl_idx].spaces = len;
    p->levels[p->lvl_idx].ncount = 0;
    p->levels[p->lvl_idx].domain = syck_strndup( p->levels[p->lvl_idx-1].domain, strlen( p->levels[p->lvl_idx-1].domain ) );
    p->levels[p->lvl_idx].status = status;
    p->lvl_idx += 1;
}

void
free_any_io( SyckParser *p )
{
    ASSERT( p != NULL );
    switch ( p->io_type )
    {
        case syck_io_str:
            if ( p->io.str != NULL )
            {
                S_FREE( p->io.str );
                p->io.str = NULL;
            }
        break;

        case syck_io_file:
            if ( p->io.file != NULL )
            {
                S_FREE( p->io.file );
                p->io.file = NULL;
            }
        break;
    }
}

long
syck_move_tokens( SyckParser *p )
{
    long count, skip;
    ASSERT( p->buffer != NULL );

    if ( p->token == NULL )
        return 0;

    skip = p->limit - p->token;
    if ( ( count = p->token - p->buffer ) )
    {
	if (skip > 0)
	    S_MEMMOVE( p->buffer, p->token, char, skip );
        p->token = p->buffer;
        p->marker -= count;
        p->cursor -= count;
        p->toktmp -= count;
        p->limit -= count;
        p->lineptr -= count;
        p->linectptr -= count;
    }
    return skip;
}

void
syck_check_limit( SyckParser *p, long len )
{
    if ( p->cursor == NULL )
    {
        p->cursor = p->buffer;
        p->lineptr = p->buffer;
        p->linectptr = p->buffer;
        p->marker = p->buffer;
    }
    p->limit = p->buffer + len;
}

long
syck_parser_read( SyckParser *p )
{
    long len = 0;
    long skip = 0;
    ASSERT( p != NULL );
    switch ( p->io_type )
    {
        case syck_io_str:
            skip = syck_move_tokens( p );
            len = (p->io.str->read)( p->buffer, p->io.str, SYCK_BUFFERSIZE - 1, skip );
            break;

        case syck_io_file:
            skip = syck_move_tokens( p );
            len = (p->io.file->read)( p->buffer, p->io.file, SYCK_BUFFERSIZE - 1, skip );
            break;
    }
    syck_check_limit( p, len );
    return len;
}

long
syck_parser_readlen( SyckParser *p, long max_size )
{
    long len = 0;
    long skip = 0;
    ASSERT( p != NULL );
    switch ( p->io_type )
    {
        case syck_io_str:
            skip = syck_move_tokens( p );
            len = (p->io.str->read)( p->buffer, p->io.str, max_size, skip );
            break;

        case syck_io_file:
            skip = syck_move_tokens( p );
            len = (p->io.file->read)( p->buffer, p->io.file, max_size, skip );
            break;
    }
    syck_check_limit( p, len );
    return len;
}

SYMID
syck_parse( SyckParser *p )
{
    ASSERT( p != NULL );

    syck_st_free( p );
    syck_parser_reset_levels( p );
    syckparse( p );
    return p->root;
}

void
syck_default_error_handler( SyckParser *p, const char *msg )
{
    printf( "Error at [Line %d, Col %"PRIdPTRDIFF"]: %s\n",
        p->linect,
        p->cursor - p->lineptr,
        msg );
}

