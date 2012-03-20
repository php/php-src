/*
 * emitter.c
 *
 * $Author$
 *
 * Copyright (C) 2003 why the lucky stiff
 *
 * All Base64 code from Ruby's pack.c.
 * Ruby is Copyright (C) 1993-2007 Yukihiro Matsumoto
 */
#include "ruby/ruby.h"

#include <stdio.h>
#include <string.h>

#include "syck.h"

#define DEFAULT_ANCHOR_FORMAT "id%03d"

const char hex_table[] =
"0123456789ABCDEF";
static char b64_table[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Built-in base64 (from Ruby's pack.c)
 */
char *
syck_base64enc( char *s, long len )
{
    long i = 0;
    int padding = '=';
    char *buff = S_ALLOC_N(char, len * 4 / 3 + 6);

    while (len >= 3) {
        buff[i++] = b64_table[077 & (*s >> 2)];
        buff[i++] = b64_table[077 & (((*s << 4) & 060) | ((s[1] >> 4) & 017))];
        buff[i++] = b64_table[077 & (((s[1] << 2) & 074) | ((s[2] >> 6) & 03))];
        buff[i++] = b64_table[077 & s[2]];
        s += 3;
        len -= 3;
    }
    if (len == 2) {
        buff[i++] = b64_table[077 & (*s >> 2)];
        buff[i++] = b64_table[077 & (((*s << 4) & 060) | ((s[1] >> 4) & 017))];
        buff[i++] = b64_table[077 & (((s[1] << 2) & 074) | (('\0' >> 6) & 03))];
        buff[i++] = padding;
    }
    else if (len == 1) {
        buff[i++] = b64_table[077 & (*s >> 2)];
        buff[i++] = b64_table[077 & (((*s << 4) & 060) | (('\0' >> 4) & 017))];
        buff[i++] = padding;
        buff[i++] = padding;
    }
    buff[i++] = '\n';
    return buff;
}

char *
syck_base64dec( char *s, long len )
{
    int a = -1,b = -1,c = 0,d;
    static int first = 1;
    static int b64_xtable[256];
    char *ptr = syck_strndup( s, len );
    char *end = ptr;
    char *send = s + len;

    if (first) {
        int i;
        first = 0;

        for (i = 0; i < 256; i++) {
        b64_xtable[i] = -1;
        }
        for (i = 0; i < 64; i++) {
        b64_xtable[(int)b64_table[i]] = i;
        }
    }
    while (s < send) {
        while (s[0] == '\r' || s[0] == '\n') { s++; }
        if ((a = b64_xtable[(int)s[0]]) == -1) break;
        if ((b = b64_xtable[(int)s[1]]) == -1) break;
        if ((c = b64_xtable[(int)s[2]]) == -1) break;
        if ((d = b64_xtable[(int)s[3]]) == -1) break;
        *end++ = a << 2 | b >> 4;
        *end++ = b << 4 | c >> 2;
        *end++ = c << 6 | d;
        s += 4;
    }
    if (a != -1 && b != -1) {
        if (s + 2 < send && s[2] == '=')
        *end++ = a << 2 | b >> 4;
        if (c != -1 && s + 3 < send && s[3] == '=') {
        *end++ = a << 2 | b >> 4;
        *end++ = b << 4 | c >> 2;
        }
    }
    *end = '\0';
    /*RSTRING_LEN(buf) = ptr - RSTRING_PTR(buf);*/
    return ptr;
}

/*
 * Allocate an emitter
 */
SyckEmitter *
syck_new_emitter(void)
{
    SyckEmitter *e;
    e = S_ALLOC( SyckEmitter );
    e->headless = 0;
    e->use_header = 0;
    e->use_version = 0;
    e->sort_keys = 0;
    e->anchor_format = NULL;
    e->explicit_typing = 0;
    e->best_width = 80;
    e->style = scalar_none;
    e->stage = doc_open;
    e->indent = 2;
    e->level = -1;
    e->anchors = NULL;
    e->markers = NULL;
    e->anchored = NULL;
    e->bufsize = SYCK_BUFFERSIZE;
    e->buffer = NULL;
    e->marker = NULL;
    e->bufpos = 0;
    e->emitter_handler = NULL;
    e->output_handler = NULL;
    e->lvl_idx = 0;
    e->lvl_capa = ALLOC_CT;
    e->levels = S_ALLOC_N( SyckLevel, e->lvl_capa );
    syck_emitter_reset_levels( e );
    e->bonus = NULL;
    return e;
}

int
syck_st_free_anchors( char *key, char *name, char *arg )
{
    S_FREE( name );
    return ST_CONTINUE;
}

void
syck_emitter_st_free( SyckEmitter *e )
{
    /*
     * Free the anchor tables
     */
    if ( e->anchors != NULL )
    {
        st_foreach( e->anchors, syck_st_free_anchors, 0 );
        st_free_table( e->anchors );
        e->anchors = NULL;
    }

    if ( e->anchored != NULL )
    {
        st_free_table( e->anchored );
        e->anchored = NULL;
    }

    /*
     * Free the markers tables
     */
    if ( e->markers != NULL )
    {
        st_free_table( e->markers );
        e->markers = NULL;
    }
}

SyckLevel *
syck_emitter_current_level( SyckEmitter *e )
{
    return &e->levels[e->lvl_idx-1];
}

SyckLevel *
syck_emitter_parent_level( SyckEmitter *e )
{
    return &e->levels[e->lvl_idx-2];
}

void
syck_emitter_pop_level( SyckEmitter *e )
{
    ASSERT( e != NULL );

    /* The root level should never be popped */
    if ( e->lvl_idx <= 1 ) return;

    e->lvl_idx -= 1;
    free( e->levels[e->lvl_idx].domain );
}

void
syck_emitter_add_level( SyckEmitter *e, int len, enum syck_level_status status )
{
    ASSERT( e != NULL );
    if ( e->lvl_idx + 1 > e->lvl_capa )
    {
        e->lvl_capa += ALLOC_CT;
        S_REALLOC_N( e->levels, SyckLevel, e->lvl_capa );
    }

    ASSERT( len > e->levels[e->lvl_idx-1].spaces );
    e->levels[e->lvl_idx].spaces = len;
    e->levels[e->lvl_idx].ncount = 0;
    e->levels[e->lvl_idx].domain = syck_strndup( e->levels[e->lvl_idx-1].domain, strlen( e->levels[e->lvl_idx-1].domain ) );
    e->levels[e->lvl_idx].status = status;
    e->levels[e->lvl_idx].anctag = 0;
    e->lvl_idx += 1;
}

void
syck_emitter_reset_levels( SyckEmitter *e )
{
    while ( e->lvl_idx > 1 )
    {
        syck_emitter_pop_level( e );
    }

    if ( e->lvl_idx < 1 )
    {
        e->lvl_idx = 1;
        e->levels[0].spaces = -1;
        e->levels[0].ncount = 0;
        e->levels[0].domain = syck_strndup( "", 0 );
        e->levels[0].anctag = 0;
    }
    e->levels[0].status = syck_lvl_header;
}

void
syck_emitter_handler( SyckEmitter *e, SyckEmitterHandler hdlr )
{
    e->emitter_handler = hdlr;
}

void
syck_output_handler( SyckEmitter *e, SyckOutputHandler hdlr )
{
    e->output_handler = hdlr;
}

void
syck_free_emitter( SyckEmitter *e )
{
    /*
     * Free tables
     */
    syck_emitter_st_free( e );
    syck_emitter_reset_levels( e );
    S_FREE( e->levels[0].domain );
    S_FREE( e->levels );
    if ( e->buffer != NULL )
    {
        S_FREE( e->buffer );
    }
    S_FREE( e );
}

void
syck_emitter_clear( SyckEmitter *e )
{
    if ( e->buffer == NULL )
    {
        e->buffer = S_ALLOC_N( char, e->bufsize );
        S_MEMZERO( e->buffer, char, e->bufsize );
    }
    e->buffer[0] = '\0';
    e->marker = e->buffer;
    e->bufpos = 0;
}

/*
 * Raw write to the emitter buffer.
 */
void
syck_emitter_write( SyckEmitter *e, const char *str, long len )
{
    long at;
    ASSERT( str != NULL );
    if ( e->buffer == NULL )
    {
        syck_emitter_clear( e );
    }

    /*
     * Flush if at end of buffer
     */
    at = e->marker - e->buffer;
    if ( len + at >= (long)e->bufsize )
    {
        syck_emitter_flush( e, 0 );
	for (;;) {
	    long rest = e->bufsize - (e->marker - e->buffer);
	    if (len <= rest) break;
	    S_MEMCPY( e->marker, str, char, rest );
	    e->marker += rest;
	    str += rest;
	    len -= rest;
	    syck_emitter_flush( e, 0 );
	}
    }

    /*
     * Write to buffer
     */
    S_MEMCPY( e->marker, str, char, len );
    e->marker += len;
}

/*
 * Write a chunk of data out.
 */
void
syck_emitter_flush( SyckEmitter *e, long check_room )
{
    /*
     * Check for enough space in the buffer for check_room length.
     */
    if ( check_room > 0 )
    {
        if ( (long)e->bufsize > ( e->marker - e->buffer ) + check_room )
        {
            return;
        }
    }
    else
    {
        check_room = e->bufsize;
    }

    /*
     * Commit buffer.
     */
    if ( check_room > e->marker - e->buffer )
    {
        check_room = e->marker - e->buffer;
    }
    (e->output_handler)( e, e->buffer, check_room );
    e->bufpos += check_room;
    e->marker -= check_room;
}

/*
 * Start emitting from the given node, check for anchoring and then
 * issue the callback to the emitter handler.
 */
void
syck_emit( SyckEmitter *e, st_data_t n )
{
    SYMID oid;
    char *anchor_name = NULL;
    int indent = 0;
    long x = 0;
    SyckLevel *lvl = syck_emitter_current_level( e );

    /*
     * Determine headers.
     */
    if ( e->stage == doc_open && ( e->headless == 0 || e->use_header == 1 ) )
    {
        if ( e->use_version == 1 )
        {
            char *header = S_ALLOC_N( char, 64 );
            S_MEMZERO( header, char, 64 );
            sprintf( header, "--- %%YAML:%d.%d ", SYCK_YAML_MAJOR, SYCK_YAML_MINOR );
            syck_emitter_write( e, header, strlen( header ) );
            S_FREE( header );
        }
        else
        {
            syck_emitter_write( e, "--- ", 4 );
        }
        e->stage = doc_processing;
    }

    /* Add new level */
    if ( lvl->spaces >= 0 ) {
        indent = lvl->spaces + e->indent;
    }
    syck_emitter_add_level( e, indent, syck_lvl_open );
    lvl = syck_emitter_current_level( e );

    /* Look for anchor */
    if ( e->anchors != NULL &&
        st_lookup( e->markers, n, (st_data_t *)&oid ) &&
        st_lookup( e->anchors, (st_data_t)oid, (void *)&anchor_name ) )
    {
        if ( e->anchored == NULL )
        {
            e->anchored = st_init_numtable();
        }

        if ( ! st_lookup( e->anchored, (st_data_t)anchor_name, (st_data_t *)&x ) )
        {
            char *an = S_ALLOC_N( char, strlen( anchor_name ) + 3 );
            sprintf( an, "&%s ", anchor_name );
            syck_emitter_write( e, an, strlen( anchor_name ) + 2 );
            free( an );

            x = 1;
            st_insert( e->anchored, (st_data_t)anchor_name, (st_data_t)x );
            lvl->anctag = 1;
        }
        else
        {
            char *an = S_ALLOC_N( char, strlen( anchor_name ) + 2 );
            sprintf( an, "*%s", anchor_name );
            syck_emitter_write( e, an, strlen( anchor_name ) + 1 );
            free( an );

            goto end_emit;
        }
    }

    (e->emitter_handler)( e, n );

    /* Pop the level */
end_emit:
    syck_emitter_pop_level( e );
    if ( e->lvl_idx == 1 ) {
        syck_emitter_write( e, "\n", 1 );
        e->headless = 0;
        e->stage = doc_open;
    }
}

/*
 * Determine what tag needs to be written, based on the taguri of the node
 * and the implicit tag which would be assigned to this node.  If a tag is
 * required, write the tag.
 */
void syck_emit_tag( SyckEmitter *e, const char *tag, const char *ignore )
{
    SyckLevel *lvl;
    if ( tag == NULL ) return;
    if ( ignore != NULL && syck_tagcmp( tag, ignore ) == 0 && e->explicit_typing == 0 ) return;
    lvl = syck_emitter_current_level( e );

    /* implicit */
    if ( strlen( tag ) == 0 ) {
        syck_emitter_write( e, "! ", 2 );

    /* global types */
    } else if ( strncmp( tag, "tag:", 4 ) == 0 ) {
        int taglen = (int)strlen( tag );
        syck_emitter_write( e, "!", 1 );
        if ( strncmp( tag + 4, YAML_DOMAIN, strlen( YAML_DOMAIN ) ) == 0 ) {
            int skip = 4 + strlen( YAML_DOMAIN ) + 1;
            syck_emitter_write( e, tag + skip, taglen - skip );
        } else {
            const char *subd = tag + 4;
            while ( *subd != ':' && *subd != '\0' ) subd++;
            if ( *subd == ':' ) {
                if ( subd - tag > ( (long)( strlen( YAML_DOMAIN ) + 5 )) &&
                     strncmp( subd - strlen( YAML_DOMAIN ), YAML_DOMAIN, strlen( YAML_DOMAIN ) ) == 0 ) {
                    syck_emitter_write( e, tag + 4, subd - strlen( YAML_DOMAIN ) - ( tag + 4 ) - 1 );
                    syck_emitter_write( e, "/", 1 );
                    syck_emitter_write( e, subd + 1, ( tag + taglen ) - ( subd + 1 ) );
                } else {
                    syck_emitter_write( e, tag + 4, subd - ( tag + 4 ) );
                    syck_emitter_write( e, "/", 1 );
                    syck_emitter_write( e, subd + 1, ( tag + taglen ) - ( subd + 1 ) );
                }
            } else {
                /* TODO: Invalid tag (no colon after domain) */
                return;
            }
        }
        syck_emitter_write( e, " ", 1 );

    /* private types */
    } else if ( strncmp( tag, "x-private:", 10 ) == 0 ) {
        syck_emitter_write( e, "!!", 2 );
        syck_emitter_write( e, tag + 10, strlen( tag ) - 10 );
        syck_emitter_write( e, " ", 1 );
    }
    lvl->anctag = 1;
}

/*
 * Emit a newline and an appropriately spaced indent.
 */
void syck_emit_indent( SyckEmitter *e )
{
    int i;
    SyckLevel *lvl = syck_emitter_current_level( e );
    if ( e->bufpos == 0 && ( e->marker - e->buffer ) == 0 ) return;
    if ( lvl->spaces >= 0 ) {
        char *spcs = S_ALLOC_N( char, lvl->spaces + 2 );

        spcs[0] = '\n'; spcs[lvl->spaces + 1] = '\0';
        for ( i = 0; i < lvl->spaces; i++ ) spcs[i+1] = ' ';
        syck_emitter_write( e, spcs, lvl->spaces + 1 );
        free( spcs );
    }
}

/* Clear the scan */
#define SCAN_NONE       0
/* All printable characters? */
#define SCAN_NONPRINT   1
/* Any indented lines? */
#define SCAN_INDENTED   2
/* Larger than the requested width? */
#define SCAN_WIDE       4
/* Opens or closes with whitespace? */
#define SCAN_WHITEEDGE  8
/* Contains a newline */
#define SCAN_NEWLINE    16
/* Contains a single quote */
#define SCAN_SINGLEQ    32
/* Contains a double quote */
#define SCAN_DOUBLEQ    64
/* Starts with a token */
#define SCAN_INDIC_S    128
/* Contains a flow indicator */
#define SCAN_INDIC_C    256
/* Ends without newlines */
#define SCAN_NONL_E     512
/* Ends with many newlines */
#define SCAN_MANYNL_E   1024
/* Contains flow map indicators */
#define SCAN_FLOWMAP    2048
/* Contains flow seq indicators */
#define SCAN_FLOWSEQ    4096
/* Contains a valid doc separator */
#define SCAN_DOCSEP     8192

/*
 * Basic printable test for LATIN-1 characters.
 */
int
syck_scan_scalar( int req_width, const char *cursor, long len )
{
    long i = 0, start = 0;
    int flags = SCAN_NONE;

    if ( len < 1 )  return flags;

    /* c-indicators from the spec */
    if ( cursor[0] == '[' || cursor[0] == ']' ||
         cursor[0] == '{' || cursor[0] == '}' ||
         cursor[0] == '!' || cursor[0] == '*' ||
         cursor[0] == '&' || cursor[0] == '|' ||
         cursor[0] == '>' || cursor[0] == '\'' ||
         cursor[0] == '"' || cursor[0] == '#' ||
         cursor[0] == '%' || cursor[0] == '@' ||
         cursor[0] == '&' ) {
            flags |= SCAN_INDIC_S;
    }
    if ( ( cursor[0] == '-' || cursor[0] == ':' ||
           cursor[0] == '?' || cursor[0] == ',' ) &&
           ( len == 1 || cursor[1] == ' ' || cursor[1] == '\n' ) )
    {
            flags |= SCAN_INDIC_S;
    }

    /* whitespace edges */
    if ( cursor[len-1] != '\n' ) {
        flags |= SCAN_NONL_E;
    } else if ( len > 1 && cursor[len-2] == '\n' ) {
        flags |= SCAN_MANYNL_E;
    }
    if (
        ( len > 0 && ( cursor[0] == ' ' || cursor[0] == '\t' || cursor[0] == '\n' || cursor[0] == '\r' ) ) ||
        ( len > 1 && ( cursor[len-1] == ' ' || cursor[len-1] == '\t' ) )
    ) {
        flags |= SCAN_WHITEEDGE;
    }

    /* opening doc sep */
    if ( len >= 3 && strncmp( cursor, "---", 3 ) == 0 )
        flags |= SCAN_DOCSEP;

    /* scan string */
    for ( i = 0; i < len; i++ ) {

        if ( ! ( cursor[i] == 0x9 ||
                 cursor[i] == 0xA ||
                 cursor[i] == 0xD ||
               ( cursor[i] >= 0x20 && cursor[i] <= 0x7E ) )
        ) {
            flags |= SCAN_NONPRINT;
        }
        else if ( cursor[i] == '\n' ) {
            flags |= SCAN_NEWLINE;
            if ( len - i >= 3 && strncmp( &cursor[i+1], "---", 3 ) == 0 )
                flags |= SCAN_DOCSEP;
            if ( cursor[i+1] == ' ' || cursor[i+1] == '\t' )
                flags |= SCAN_INDENTED;
            if ( req_width > 0 && i - start > req_width )
                flags |= SCAN_WIDE;
            start = i;
        }
        else if ( cursor[i] == '\'' )
        {
            flags |= SCAN_SINGLEQ;
        }
        else if ( cursor[i] == '"' )
        {
            flags |= SCAN_DOUBLEQ;
        }
        else if ( cursor[i] == ']' )
        {
            flags |= SCAN_FLOWSEQ;
        }
        else if ( cursor[i] == '}' )
        {
            flags |= SCAN_FLOWMAP;
        }
        /* remember, if plain collections get implemented, to add nb-plain-flow-char */
        else if ( ( cursor[i] == ' ' && cursor[i+1] == '#' ) ||
                  ( cursor[i] == ':' &&
                    ( cursor[i+1] == ' ' || cursor[i+1] == '\n' || i == len - 1 ) ) )
        {
            flags |= SCAN_INDIC_C;
        }
        else if ( cursor[i] == ',' &&
                  ( cursor[i+1] == ' ' || cursor[i+1] == '\n' || i == len - 1 ) )
        {
            flags |= SCAN_FLOWMAP;
            flags |= SCAN_FLOWSEQ;
        }
    }

    /* printf( "---STR---\n%s\nFLAGS: %d\n", cursor, flags ); */
    return flags;
}
/*
 * All scalars should be emitted through this function, which determines an appropriate style,
 * tag and indent.
 */
void syck_emit_scalar( SyckEmitter *e, const char *tag, enum scalar_style force_style, int force_indent, int force_width,
                       char keep_nl, const char *str, long len )
{
    enum scalar_style favor_style = scalar_literal;
    SyckLevel *parent = syck_emitter_parent_level( e );
    SyckLevel *lvl = syck_emitter_current_level( e );
    int scan = 0;
    const char *match_implicit;
    char *implicit;

    if ( str == NULL ) str = "";

    /* No empty nulls as map keys */
    if ( len == 0 && ( parent->status == syck_lvl_map || parent->status == syck_lvl_imap ) &&
         parent->ncount % 2 == 1 && syck_tagcmp( tag, "tag:yaml.org,2002:null" ) == 0 )
    {
        str = "~";
        len = 1;
    }

    scan = syck_scan_scalar( force_width, str, len );
    match_implicit = syck_match_implicit( str, len );

    /* quote strings which default to implicits */
    implicit = syck_taguri( YAML_DOMAIN, match_implicit, (int)strlen( match_implicit ) );
    if ( syck_tagcmp( tag, implicit ) != 0 && syck_tagcmp( tag, "tag:yaml.org,2002:str" ) == 0 ) {
        force_style = scalar_2quote;
    } else {
        /* complex key */
        if ( parent->status == syck_lvl_map && parent->ncount % 2 == 1 &&
             ( !( tag == NULL ||
             ( implicit != NULL && syck_tagcmp( tag, implicit ) == 0 && e->explicit_typing == 0 ) ) ) )
        {
            syck_emitter_write( e, "? ", 2 );
            parent->status = syck_lvl_mapx;
        }
        syck_emit_tag( e, tag, implicit );
    }
    S_FREE( implicit );

    /* if still arbitrary, sniff a good block style. */
    if ( force_style == scalar_none ) {
        if ( scan & SCAN_NEWLINE ) {
            force_style = scalar_literal;
        } else {
            force_style = scalar_plain;
        }
    }

    if ( e->style == scalar_fold ) {
        favor_style = scalar_fold;
    }

    /* Determine block style */
    if ( scan & SCAN_NONPRINT ) {
        force_style = scalar_2quote;
    } else if ( scan & SCAN_WHITEEDGE ) {
        force_style = scalar_2quote;
    } else if ( force_style != scalar_fold && ( scan & SCAN_INDENTED ) ) {
        force_style = scalar_literal;
    } else if ( force_style == scalar_plain && ( scan & SCAN_NEWLINE ) ) {
        force_style = favor_style;
    } else if ( force_style == scalar_plain && parent->status == syck_lvl_iseq && ( scan & SCAN_FLOWSEQ ) ) {
        force_style = scalar_2quote;
    } else if ( force_style == scalar_plain && parent->status == syck_lvl_imap && ( scan & SCAN_FLOWMAP ) ) {
        force_style = scalar_2quote;
    /* } else if ( force_style == scalar_fold && ( ! ( scan & SCAN_WIDE ) ) ) {
        force_style = scalar_literal; */
    } else if ( force_style == scalar_plain && ( scan & SCAN_INDIC_S || scan & SCAN_INDIC_C ) ) {
        if ( scan & SCAN_NEWLINE ) {
            force_style = favor_style;
        } else {
            force_style = scalar_2quote;
        }
    }

    if ( force_indent > 0 ) {
        lvl->spaces = parent->spaces + force_indent;
    } else if ( scan & SCAN_DOCSEP ) {
        lvl->spaces = parent->spaces + e->indent;
    }

    /* For now, all ambiguous keys are going to be double-quoted */
    if ( ( parent->status == syck_lvl_map || parent->status == syck_lvl_mapx ) && parent->ncount % 2 == 1 ) {
        if ( force_style != scalar_plain ) {
            force_style = scalar_2quote;
        }
    }

    /* If the parent is an inline, double quote anything complex */
    if ( parent->status == syck_lvl_imap || parent->status == syck_lvl_iseq ) {
        if ( force_style != scalar_plain && force_style != scalar_1quote ) {
            force_style = scalar_2quote;
        }
    }

    /* Fix the ending newlines */
    if ( scan & SCAN_NONL_E ) {
        keep_nl = NL_CHOMP;
    } else if ( scan & SCAN_MANYNL_E ) {
        keep_nl = NL_KEEP;
    }

    /* Write the text node */
    switch ( force_style )
    {
        case scalar_1quote:
            syck_emit_1quoted( e, force_width, str, len );
        break;

        case scalar_none:
        case scalar_2quote:
            syck_emit_2quoted( e, force_width, str, len );
        break;

        case scalar_fold:
            syck_emit_folded( e, force_width, keep_nl, str, len );
        break;

        case scalar_literal:
            syck_emit_literal( e, keep_nl, str, len );
        break;

        case scalar_plain:
            syck_emitter_write( e, str, len );
        break;
    }

    if ( parent->status == syck_lvl_mapx )
    {
        syck_emitter_write( e, "\n", 1 );
    }
}

void
syck_emitter_escape( SyckEmitter *e, const char *src, long len )
{
    int i;
    for( i = 0; i < len; i++ )
    {
        if( (src[i] < 0x20) || (0x7E < src[i]) )
        {
            syck_emitter_write( e, "\\", 1 );
            if( '\0' == src[i] )
                syck_emitter_write( e, "0", 1 );
            else
            {
                syck_emitter_write( e, "x", 1 );
                syck_emitter_write( e, (const char *)hex_table + ((src[i] & 0xF0) >> 4), 1 );
                syck_emitter_write( e, (const char *)hex_table + (src[i] & 0x0F), 1 );
            }
        }
        else
        {
            syck_emitter_write( e, src + i, 1 );
            if( '\\' == src[i] )
                syck_emitter_write( e, "\\", 1 );
        }
    }
}

/*
 * Outputs a single-quoted block.
 */
void
syck_emit_1quoted( SyckEmitter *e, int width, const char *str, long len )
{
    char do_indent = 0;
    const char *mark = str;
    const char *start = str;
    const char *end = str;
    syck_emitter_write( e, "'", 1 );
    while ( mark < str + len ) {
        if ( do_indent ) {
            syck_emit_indent( e );
            do_indent = 0;
        }
        switch ( *mark ) {
            case '\'':  syck_emitter_write( e, "'", 1 ); break;

            case '\n':
                end = mark + 1;
                if ( *start != ' ' && *start != '\n' && *end != '\n' && *end != ' ' ) {
                    syck_emitter_write( e, "\n\n", 2 );
                } else {
                    syck_emitter_write( e, "\n", 1 );
                }
                do_indent = 1;
                start = mark + 1;
            break;

            case ' ':
                if ( width > 0 && *start != ' ' && mark - end > width ) {
                    do_indent = 1;
                    end = mark + 1;
                } else {
                    syck_emitter_write( e, " ", 1 );
                }
            break;

            default:
                syck_emitter_write( e, mark, 1 );
            break;
        }
        mark++;
    }
    syck_emitter_write( e, "'", 1 );
}

/*
 * Outputs a double-quoted block.
 */
void
syck_emit_2quoted( SyckEmitter *e, int width, const char *str, long len )
{
    char do_indent = 0;
    const char *mark = str;
    const char *start = str;
    const char *end = str;
    syck_emitter_write( e, "\"", 1 );
    while ( mark < str + len ) {
        if ( do_indent > 0 ) {
            if ( do_indent == 2 ) {
                syck_emitter_write( e, "\\", 1 );
            }
            syck_emit_indent( e );
            do_indent = 0;
        }
        switch ( *mark ) {

            /* Escape sequences allowed within double quotes. */
            case '"':  syck_emitter_write( e, "\\\"", 2 ); break;
            case '\\': syck_emitter_write( e, "\\\\", 2 ); break;
            case '\0': syck_emitter_write( e, "\\0",  2 ); break;
            case '\a': syck_emitter_write( e, "\\a",  2 ); break;
            case '\b': syck_emitter_write( e, "\\b",  2 ); break;
            case '\f': syck_emitter_write( e, "\\f",  2 ); break;
            case '\r': syck_emitter_write( e, "\\r",  2 ); break;
            case '\t': syck_emitter_write( e, "\\t",  2 ); break;
            case '\v': syck_emitter_write( e, "\\v",  2 ); break;
            case 0x1b: syck_emitter_write( e, "\\e",  2 ); break;

            case '\n':
                end = mark + 1;
                syck_emitter_write( e, "\\n", 2 );
                do_indent = 2;
                start = mark + 1;
                if ( start < str + len && ( *start == ' ' || *start == '\n' ) ) {
                    do_indent = 0;
                }
            break;

            case ' ':
                if ( width > 0 && *start != ' ' && mark - end > width ) {
                    do_indent = 1;
                    end = mark + 1;
                } else {
                    syck_emitter_write( e, " ", 1 );
                }
            break;

            default:
                syck_emitter_escape( e, mark, 1 );
            break;
        }
        mark++;
    }
    syck_emitter_write( e, "\"", 1 );
}

/*
 * Outputs a literal block.
 */
void
syck_emit_literal( SyckEmitter *e, char keep_nl, const char *str, long len )
{
    const char *mark = str;
    const char *start = str;
    const char *end = str;
    syck_emitter_write( e, "|", 1 );
    if ( keep_nl == NL_CHOMP ) {
        syck_emitter_write( e, "-", 1 );
    } else if ( keep_nl == NL_KEEP ) {
        syck_emitter_write( e, "+", 1 );
    }
    syck_emit_indent( e );
    while ( mark < str + len ) {
        if ( *mark == '\n' ) {
            end = mark;
            if ( *start != ' ' && *start != '\n' && *end != '\n' && *end != ' ' ) end += 1;
            syck_emitter_write( e, start, end - start );
            if ( mark + 1 == str + len ) {
                if ( keep_nl != NL_KEEP ) syck_emitter_write( e, "\n", 1 );
            } else {
                syck_emit_indent( e );
            }
            start = mark + 1;
        }
        mark++;
    }
    end = str + len;
    if ( start < end ) {
        syck_emitter_write( e, start, end - start );
    }
}

/*
 * Outputs a folded block.
 */
void
syck_emit_folded( SyckEmitter *e, int width, char keep_nl, const char *str, long len )
{
    const char *mark = str;
    const char *start = str;
    const char *end = str;
    syck_emitter_write( e, ">", 1 );
    if ( keep_nl == NL_CHOMP ) {
        syck_emitter_write( e, "-", 1 );
    } else if ( keep_nl == NL_KEEP ) {
        syck_emitter_write( e, "+", 1 );
    }
    syck_emit_indent( e );
    if ( width <= 0 ) width = e->best_width;
    while ( mark < str + len ) {
        switch ( *mark ) {
            case '\n':
                syck_emitter_write( e, end, mark - end );
                end = mark + 1;
                if ( *start != ' ' && *start != '\n' && *end != '\n' && *end != ' ' ) {
                    syck_emitter_write( e, "\n", 1 );
                }
                if ( mark + 1 == str + len ) {
                    if ( keep_nl != NL_KEEP ) syck_emitter_write( e, "\n", 1 );
                } else {
                    syck_emit_indent( e );
                }
                start = mark + 1;
            break;

            case ' ':
                if ( *start != ' ' ) {
                    if ( mark - end > width ) {
                        syck_emitter_write( e, end, mark - end );
                        syck_emit_indent( e );
                        end = mark + 1;
                    }
                }
            break;
        }
        mark++;
    }
    if ( end < mark ) {
        syck_emitter_write( e, end, mark - end );
    }
}

/*
 * Begins emission of a sequence.
 */
void syck_emit_seq( SyckEmitter *e, const char *tag, enum seq_style style )
{
    SyckLevel *parent = syck_emitter_parent_level( e );
    SyckLevel *lvl = syck_emitter_current_level( e );
    syck_emit_tag( e, tag, "tag:yaml.org,2002:seq" );
    if ( style == seq_inline || ( parent->status == syck_lvl_imap || parent->status == syck_lvl_iseq ) ) {
        syck_emitter_write( e, "[", 1 );
        lvl->status = syck_lvl_iseq;
    } else {
        /* complex key */
        if ( parent->status == syck_lvl_map && parent->ncount % 2 == 1 ) {
            syck_emitter_write( e, "? ", 2 );
            parent->status = syck_lvl_mapx;
        }
        lvl->status = syck_lvl_seq;
    }
}

/*
 * Begins emission of a mapping.
 */
void
syck_emit_map( SyckEmitter *e, const char *tag, enum map_style style )
{
    SyckLevel *parent = syck_emitter_parent_level( e );
    SyckLevel *lvl = syck_emitter_current_level( e );
    syck_emit_tag( e, tag, "tag:yaml.org,2002:map" );
    if ( style == map_inline || ( parent->status == syck_lvl_imap || parent->status == syck_lvl_iseq ) ) {
        syck_emitter_write( e, "{", 1 );
        lvl->status = syck_lvl_imap;
    } else {
        /* complex key */
        if ( parent->status == syck_lvl_map && parent->ncount % 2 == 1 ) {
            syck_emitter_write( e, "? ", 2 );
            parent->status = syck_lvl_mapx;
        }
        lvl->status = syck_lvl_map;
    }
}

/*
 * Handles emitting of a collection item (for both
 * sequences and maps)
 */
void syck_emit_item( SyckEmitter *e, st_data_t n )
{
    SyckLevel *lvl = syck_emitter_current_level( e );
    switch ( lvl->status )
    {
        case syck_lvl_seq:
        {
            SyckLevel *parent = syck_emitter_parent_level( e );

            /* seq-in-map shortcut -- the lvl->anctag check should be unneccesary but
             * there is a nasty shift/reduce in the parser on this point and
             * i'm not ready to tickle it. */
            if ( lvl->anctag == 0 && parent->status == syck_lvl_map && lvl->ncount == 0 ) {
                lvl->spaces = parent->spaces;
            }

            /* seq-in-seq shortcut */
            else if ( lvl->anctag == 0 && parent->status == syck_lvl_seq && lvl->ncount == 0 ) {
                int spcs = ( lvl->spaces - parent->spaces ) - 2;
                if ( spcs >= 0 ) {
                    int i = 0;
                    for ( i = 0; i < spcs; i++ ) {
                        syck_emitter_write( e, " ", 1 );
                    }
                    syck_emitter_write( e, "- ", 2 );
                    break;
                }
            }

            syck_emit_indent( e );
            syck_emitter_write( e, "- ", 2 );
        }
        break;

        case syck_lvl_iseq:
        {
            if ( lvl->ncount > 0 ) {
                syck_emitter_write( e, ", ", 2 );
            }
        }
        break;

        case syck_lvl_map:
        {
            SyckLevel *parent = syck_emitter_parent_level( e );

            /* map-in-seq shortcut */
            if ( lvl->anctag == 0 && parent->status == syck_lvl_seq && lvl->ncount == 0 ) {
                int spcs = ( lvl->spaces - parent->spaces ) - 2;
                if ( spcs >= 0 ) {
                    int i = 0;
                    for ( i = 0; i < spcs; i++ ) {
                        syck_emitter_write( e, " ", 1 );
                    }
                    break;
                }
            }

            if ( lvl->ncount % 2 == 0 ) {
                syck_emit_indent( e );
            } else {
                syck_emitter_write( e, ": ", 2 );
            }
        }
        break;

        case syck_lvl_mapx:
        {
            if ( lvl->ncount % 2 == 0 ) {
                syck_emit_indent( e );
                lvl->status = syck_lvl_map;
            } else {
                int i;
                if ( lvl->spaces > 0 ) {
                    char *spcs = S_ALLOC_N( char, lvl->spaces + 1 );

                    spcs[lvl->spaces] = '\0';
                    for ( i = 0; i < lvl->spaces; i++ ) spcs[i] = ' ';
                    syck_emitter_write( e, spcs, lvl->spaces );
                    S_FREE( spcs );
                }
                syck_emitter_write( e, ": ", 2 );
            }
        }
        break;

        case syck_lvl_imap:
        {
            if ( lvl->ncount > 0 ) {
                if ( lvl->ncount % 2 == 0 ) {
                    syck_emitter_write( e, ", ", 2 );
                } else {
                    syck_emitter_write( e, ": ", 2 );
                }
            }
        }
        break;

        default: break;
    }
    lvl->ncount++;

    syck_emit( e, n );
}

/*
 * Closes emission of a collection.
 */
void syck_emit_end( SyckEmitter *e )
{
    SyckLevel *lvl = syck_emitter_current_level( e );
    SyckLevel *parent = syck_emitter_parent_level( e );
    switch ( lvl->status )
    {
        case syck_lvl_seq:
            if ( lvl->ncount == 0 ) {
                syck_emitter_write( e, "[]\n", 3 );
            } else if ( parent->status == syck_lvl_mapx ) {
                syck_emitter_write( e, "\n", 1 );
            }
        break;

        case syck_lvl_iseq:
            syck_emitter_write( e, "]\n", 1 );
        break;

        case syck_lvl_map:
            if ( lvl->ncount == 0 ) {
                syck_emitter_write( e, "{}\n", 3 );
            } else if ( lvl->ncount % 2 == 1 ) {
                syck_emitter_write( e, ":\n", 1 );
            } else if ( parent->status == syck_lvl_mapx ) {
                syck_emitter_write( e, "\n", 1 );
            }
        break;

        case syck_lvl_imap:
            syck_emitter_write( e, "}\n", 1 );
        break;

        default: break;
    }
}

/*
 * Fill markers table with emitter nodes in the
 * soon-to-be-emitted tree.
 */
SYMID
syck_emitter_mark_node( SyckEmitter *e, st_data_t n )
{
    SYMID oid = 0;
    char *anchor_name = NULL;

    /*
     * Ensure markers table is initialized.
     */
    if ( e->markers == NULL )
    {
        e->markers = st_init_numtable();
    }

    /*
     * Markers table initially marks the string position of the
     * object.  Doesn't yet create an anchor, simply notes the
     * position.
     */
    if ( ! st_lookup( e->markers, n, (st_data_t *)&oid ) )
    {
        /*
         * Store all markers
         */
        oid = e->markers->num_entries + 1;
        st_insert( e->markers, n, (st_data_t)oid );
    }
    else
    {
        if ( e->anchors == NULL )
        {
            e->anchors = st_init_numtable();
        }

        if ( ! st_lookup( e->anchors, (st_data_t)oid, (void *)&anchor_name ) )
        {
            int idx = 0;
            const char *anc = ( e->anchor_format == NULL ? DEFAULT_ANCHOR_FORMAT : e->anchor_format );

            /*
             * Second time hitting this object, let's give it an anchor
             */
            idx = (int)(e->anchors->num_entries + 1);
            anchor_name = S_ALLOC_N( char, strlen( anc ) + 10 );
            S_MEMZERO( anchor_name, char, strlen( anc ) + 10 );
            sprintf( anchor_name, anc, idx );

            /*
             * Insert into anchors table
             */
            st_insert( e->anchors, (st_data_t)oid, (st_data_t)anchor_name );
        }
    }
    return oid;
}

