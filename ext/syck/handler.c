/*
 * handler.c
 *
 * $Author$
 *
 * Copyright (C) 2003 why the lucky stiff
 */

#include "ruby/ruby.h"
#include "syck.h"

SYMID
syck_hdlr_add_node( SyckParser *p, SyckNode *n )
{
    SYMID id;

    if ( ! n->id )
    {
        n->id = (p->handler)( p, n );
    }
    id = n->id;

    if ( n->anchor == NULL )
    {
        syck_free_node( n );
    }
    return id;
}

SyckNode *
syck_hdlr_add_anchor( SyckParser *p, char *a, SyckNode *n )
{
    SyckNode *ntmp = NULL;

    n->anchor = a;
    if ( p->bad_anchors != NULL )
    {
        SyckNode *bad;
        if ( st_lookup( p->bad_anchors, (st_data_t)a, (void *)&bad ) )
        {
            if ( n->kind != syck_str_kind )
            {
                n->id = bad->id;
                (p->handler)( p, n );
            }
        }
    }
    if ( p->anchors == NULL )
    {
        p->anchors = st_init_strtable();
    }
    if ( st_lookup( p->anchors, (st_data_t)a, (void *)&ntmp ) )
    {
        if ( ntmp != (void *)1 )
        {
            syck_free_node( ntmp );
        }
    }
    st_insert( p->anchors, (st_data_t)a, (st_data_t)n );
    return n;
}

void
syck_hdlr_remove_anchor( SyckParser *p, char *a )
{
    char *atmp = a;
    SyckNode *ntmp;
    if ( p->anchors == NULL )
    {
        p->anchors = st_init_strtable();
    }
    if ( st_delete( p->anchors, (void *)&atmp, (void *)&ntmp ) )
    {
        if ( ntmp != (void *)1 )
        {
            syck_free_node( ntmp );
        }
    }
    st_insert( p->anchors, (st_data_t)a, (st_data_t)1 );
}

SyckNode *
syck_hdlr_get_anchor( SyckParser *p, char *a )
{
    SyckNode *n = NULL;

    if ( p->anchors != NULL )
    {
        if ( st_lookup( p->anchors, (st_data_t)a, (void *)&n ) )
        {
            if ( n != (void *)1 )
            {
                S_FREE( a );
                return n;
            }
            else
            {
                if ( p->bad_anchors == NULL )
                {
                    p->bad_anchors = st_init_strtable();
                }
                if ( ! st_lookup( p->bad_anchors, (st_data_t)a, (void *)&n ) )
                {
                    n = (p->bad_anchor_handler)( p, a );
                    st_insert( p->bad_anchors, (st_data_t)a, (st_data_t)n );
                }
            }
        }
    }

    if ( n == NULL )
    {
        n = (p->bad_anchor_handler)( p, a );
    }

    if ( n->anchor )
    {
        S_FREE( a );
    }
    else
    {
        n->anchor = a;
    }

    return n;
}

void
syck_add_transfer( char *uri, SyckNode *n, int taguri )
{
    if ( n->type_id != NULL )
    {
        S_FREE( n->type_id );
    }

    if ( taguri == 0 )
    {
        n->type_id = uri;
        return;
    }

    n->type_id = syck_type_id_to_uri( uri );
    S_FREE( uri );
}

char *
syck_xprivate( const char *type_id, int type_len )
{
    char *uri = S_ALLOC_N( char, type_len + 14 );
    uri[0] = '\0';
    strcat( uri, "x-private:" );
    strncat( uri, type_id, type_len );
    return uri;
}

char *
syck_taguri( const char *domain, const char *type_id, int type_len )
{
    char *uri = S_ALLOC_N( char, strlen( domain ) + type_len + 14 );
    uri[0] = '\0';
    strcat( uri, "tag:" );
    strcat( uri, domain );
    strcat( uri, ":" );
    strncat( uri, type_id, type_len );
    return uri;
}

int
syck_try_implicit( SyckNode *n )
{
    return 1;
}

