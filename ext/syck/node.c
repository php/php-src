/*
 * node.c
 *
 * $Author$
 *
 * Copyright (C) 2003 why the lucky stiff
 */

#include "ruby/ruby.h"
#include "syck.h"

/*
 * Node allocation functions
 */
SyckNode *
syck_alloc_node( enum syck_kind_tag type )
{
    SyckNode *s;

    s = S_ALLOC( SyckNode );
    s->kind = type;
    s->id = 0;
    s->type_id = NULL;
    s->anchor = NULL;
    s->shortcut = NULL;

    return s;
}

void
syck_free_node( SyckNode *n )
{
    syck_free_members( n );
    if ( n->type_id != NULL )
    {
        S_FREE( n->type_id );
        n->type_id = NULL;
    }
    if ( n->anchor != NULL )
    {
        S_FREE( n->anchor );
        n->anchor = NULL;
    }
    S_FREE( n );
}

SyckNode *
syck_alloc_map(void)
{
    SyckNode *n;
    struct SyckMap *m;

    m = S_ALLOC( struct SyckMap );
    m->style = map_none;
    m->idx = 0;
    m->capa = ALLOC_CT;
    m->keys = S_ALLOC_N( SYMID, m->capa );
    m->values = S_ALLOC_N( SYMID, m->capa );

    n = syck_alloc_node( syck_map_kind );
    n->data.pairs = m;

    return n;
}

SyckNode *
syck_alloc_seq(void)
{
    SyckNode *n;
    struct SyckSeq *s;

    s = S_ALLOC( struct SyckSeq );
    s->style = seq_none;
    s->idx = 0;
    s->capa = ALLOC_CT;
    s->items = S_ALLOC_N( SYMID, s->capa );

    n = syck_alloc_node( syck_seq_kind );
    n->data.list = s;

    return n;
}

SyckNode *
syck_alloc_str(void)
{
    SyckNode *n;
    struct SyckStr *s;

    s = S_ALLOC( struct SyckStr );
    s->len = 0;
    s->ptr = NULL;
    s->style = scalar_none;

    n = syck_alloc_node( syck_str_kind );
    n->data.str = s;

    return n;
}

SyckNode *
syck_new_str( const char *str, enum scalar_style style )
{
    return syck_new_str2( str, strlen( str ), style );
}

SyckNode *
syck_new_str2( const char *str, long len, enum scalar_style style )
{
    SyckNode *n;

    n = syck_alloc_str();
    n->data.str->ptr = S_ALLOC_N( char, len + 1 );
    n->data.str->len = len;
    n->data.str->style = style;
    memcpy( n->data.str->ptr, str, len );
    n->data.str->ptr[len] = '\0';

    return n;
}

void
syck_replace_str( SyckNode *n, char *str, enum scalar_style style )
{
    syck_replace_str2( n, str, strlen( str ), style );
}

void
syck_replace_str2( SyckNode *n, char *str, long len, enum scalar_style style )
{
    if ( n->data.str->ptr != NULL )
    {
        S_FREE( n->data.str->ptr );
        n->data.str->ptr = NULL;
        n->data.str->len = 0;
    }
    n->data.str->ptr = S_ALLOC_N( char, len + 1 );
    n->data.str->len = len;
    n->data.str->style = style;
    memcpy( n->data.str->ptr, str, len );
    n->data.str->ptr[len] = '\0';
}

void
syck_str_blow_away_commas( SyckNode *n )
{
    char *go, *end;

    go = n->data.str->ptr;
    end = go + n->data.str->len;
    while ( *(++go) != '\0' )
    {
        if ( *go == ',' )
        {
            n->data.str->len -= 1;
            memmove( go, go + 1, end - go );
            end -= 1;
        }
    }
}

char *
syck_str_read( SyckNode *n )
{
    ASSERT( n != NULL );
    return n->data.str->ptr;
}

SyckNode *
syck_new_map( SYMID key, SYMID value )
{
    SyckNode *n;

    n = syck_alloc_map();
    syck_map_add( n, key, value );

    return n;
}

void
syck_map_empty( SyckNode *n )
{
    struct SyckMap *m;
    ASSERT( n != NULL );
    ASSERT( n->data.list != NULL );

    S_FREE( n->data.pairs->keys );
    S_FREE( n->data.pairs->values );
    m = n->data.pairs;
    m->idx = 0;
    m->capa = ALLOC_CT;
    m->keys = S_ALLOC_N( SYMID, m->capa );
    m->values = S_ALLOC_N( SYMID, m->capa );
}

void
syck_map_add( SyckNode *map, SYMID key, SYMID value )
{
    struct SyckMap *m;
    long idx;

    ASSERT( map != NULL );
    ASSERT( map->data.pairs != NULL );

    m = map->data.pairs;
    idx = m->idx;
    m->idx += 1;
    if ( m->idx > m->capa )
    {
        m->capa += ALLOC_CT;
        S_REALLOC_N( m->keys, SYMID, m->capa );
        S_REALLOC_N( m->values, SYMID, m->capa );
    }
    m->keys[idx] = key;
    m->values[idx] = value;
}

void
syck_map_update( SyckNode *map1, SyckNode *map2 )
{
    struct SyckMap *m1, *m2;
    long new_idx, new_capa;
    ASSERT( map1 != NULL );
    ASSERT( map2 != NULL );

    m1 = map1->data.pairs;
    m2 = map2->data.pairs;
    if ( m2->idx < 1 ) return;

    new_idx = m1->idx;
    new_idx += m2->idx;
    new_capa = m1->capa;
    while ( new_idx > new_capa )
    {
        new_capa += ALLOC_CT;
    }
    if ( new_capa > m1->capa )
    {
        m1->capa = new_capa;
        S_REALLOC_N( m1->keys, SYMID, m1->capa );
        S_REALLOC_N( m1->values, SYMID, m1->capa );
    }
    for ( new_idx = 0; new_idx < m2->idx; m1->idx++, new_idx++ )
    {
        m1->keys[m1->idx] = m2->keys[new_idx];
        m1->values[m1->idx] = m2->values[new_idx];
    }
}

long
syck_map_count( SyckNode *map )
{
    ASSERT( map != NULL );
    ASSERT( map->data.pairs != NULL );
    return map->data.pairs->idx;
}

void
syck_map_assign( SyckNode *map, enum map_part p, long idx, SYMID id )
{
    struct SyckMap *m;

    ASSERT( map != NULL );
    m = map->data.pairs;
    ASSERT( m != NULL );
    if ( p == map_key )
    {
        m->keys[idx] = id;
    }
    else
    {
        m->values[idx] = id;
    }
}

SYMID
syck_map_read( SyckNode *map, enum map_part p, long idx )
{
    struct SyckMap *m;

    ASSERT( map != NULL );
    m = map->data.pairs;
    ASSERT( m != NULL );
    if ( p == map_key )
    {
        return m->keys[idx];
    }
    else
    {
        return m->values[idx];
    }
}

SyckNode *
syck_new_seq( SYMID value )
{
    SyckNode *n;

    n = syck_alloc_seq();
    syck_seq_add( n, value );

    return n;
}

void
syck_seq_empty( SyckNode *n )
{
    struct SyckSeq *s;
    ASSERT( n != NULL );
    ASSERT( n->data.list != NULL );

    S_FREE( n->data.list->items );
    s = n->data.list;
    s->idx = 0;
    s->capa = ALLOC_CT;
    s->items = S_ALLOC_N( SYMID, s->capa );
}

void
syck_seq_add( SyckNode *arr, SYMID value )
{
    struct SyckSeq *s;
    long idx;

    ASSERT( arr != NULL );
    ASSERT( arr->data.list != NULL );

    s = arr->data.list;
    idx = s->idx;
    s->idx += 1;
    if ( s->idx > s->capa )
    {
        s->capa += ALLOC_CT;
        S_REALLOC_N( s->items, SYMID, s->capa );
    }
    s->items[idx] = value;
}

long
syck_seq_count( SyckNode *seq )
{
    ASSERT( seq != NULL );
    ASSERT( seq->data.list != NULL );
    return seq->data.list->idx;
}

void
syck_seq_assign( SyckNode *seq, long idx, SYMID id )
{
    struct SyckSeq *s;

    ASSERT( map != NULL );
    s = seq->data.list;
    ASSERT( m != NULL );
    s->items[idx] = id;
}

SYMID
syck_seq_read( SyckNode *seq, long idx )
{
    struct SyckSeq *s;

    ASSERT( seq != NULL );
    s = seq->data.list;
    ASSERT( s != NULL );
    return s->items[idx];
}

void
syck_free_members( SyckNode *n )
{
    if ( n == NULL ) return;

    switch ( n->kind  )
    {
        case syck_str_kind:
            if ( n->data.str != NULL )
            {
                S_FREE( n->data.str->ptr );
                n->data.str->ptr = NULL;
                n->data.str->len = 0;
                S_FREE( n->data.str );
                n->data.str = NULL;
            }
        break;

        case syck_seq_kind:
            if ( n->data.list != NULL )
            {
                S_FREE( n->data.list->items );
                S_FREE( n->data.list );
                n->data.list = NULL;
            }
        break;

        case syck_map_kind:
            if ( n->data.pairs != NULL )
            {
                S_FREE( n->data.pairs->keys );
                S_FREE( n->data.pairs->values );
                S_FREE( n->data.pairs );
                n->data.pairs = NULL;
            }
        break;
    }
}

