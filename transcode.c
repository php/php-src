/**********************************************************************

  transcode.c -

  $Author$
  created at: Tue Oct 30 16:10:22 JST 2007

  Copyright (C) 2007 Martin Duerst

**********************************************************************/

#include "ruby/ruby.h"
#include "ruby/encoding.h"
#include "internal.h"
#include "transcode_data.h"
#include <ctype.h>

#define ENABLE_ECONV_NEWLINE_OPTION 1

/* VALUE rb_cEncoding = rb_define_class("Encoding", rb_cObject); */
VALUE rb_eUndefinedConversionError;
VALUE rb_eInvalidByteSequenceError;
VALUE rb_eConverterNotFoundError;

VALUE rb_cEncodingConverter;

static VALUE sym_invalid, sym_undef, sym_replace, sym_fallback, sym_aref;
static VALUE sym_xml, sym_text, sym_attr;
static VALUE sym_universal_newline;
static VALUE sym_crlf_newline;
static VALUE sym_cr_newline;
#ifdef ENABLE_ECONV_NEWLINE_OPTION
static VALUE sym_newline, sym_universal, sym_crlf, sym_cr, sym_lf;
#endif
static VALUE sym_partial_input;

static VALUE sym_invalid_byte_sequence;
static VALUE sym_undefined_conversion;
static VALUE sym_destination_buffer_full;
static VALUE sym_source_buffer_empty;
static VALUE sym_finished;
static VALUE sym_after_output;
static VALUE sym_incomplete_input;

static unsigned char *
allocate_converted_string(const char *sname, const char *dname,
        const unsigned char *str, size_t len,
        unsigned char *caller_dst_buf, size_t caller_dst_bufsize,
        size_t *dst_len_ptr);

/* dynamic structure, one per conversion (similar to iconv_t) */
/* may carry conversion state (e.g. for iso-2022-jp) */
typedef struct rb_transcoding {
    const rb_transcoder *transcoder;

    int flags;

    int resume_position;
    unsigned int next_table;
    VALUE next_info;
    unsigned char next_byte;
    unsigned int output_index;

    ssize_t recognized_len; /* already interpreted */
    ssize_t readagain_len; /* not yet interpreted */
    union {
        unsigned char ary[8]; /* max_input <= sizeof(ary) */
        unsigned char *ptr; /* length: max_input */
    } readbuf; /* recognized_len + readagain_len used */

    ssize_t writebuf_off;
    ssize_t writebuf_len;
    union {
        unsigned char ary[8]; /* max_output <= sizeof(ary) */
        unsigned char *ptr; /* length: max_output */
    } writebuf;

    union rb_transcoding_state_t { /* opaque data for stateful encoding */
        void *ptr;
        char ary[sizeof(double) > sizeof(void*) ? sizeof(double) : sizeof(void*)];
        double dummy_for_alignment;
    } state;
} rb_transcoding;
#define TRANSCODING_READBUF(tc) \
    ((tc)->transcoder->max_input <= (int)sizeof((tc)->readbuf.ary) ? \
     (tc)->readbuf.ary : \
     (tc)->readbuf.ptr)
#define TRANSCODING_WRITEBUF(tc) \
    ((tc)->transcoder->max_output <= (int)sizeof((tc)->writebuf.ary) ? \
     (tc)->writebuf.ary : \
     (tc)->writebuf.ptr)
#define TRANSCODING_WRITEBUF_SIZE(tc) \
    ((tc)->transcoder->max_output <= (int)sizeof((tc)->writebuf.ary) ? \
     sizeof((tc)->writebuf.ary) : \
     (size_t)(tc)->transcoder->max_output)
#define TRANSCODING_STATE_EMBED_MAX ((int)sizeof(union rb_transcoding_state_t))
#define TRANSCODING_STATE(tc) \
    ((tc)->transcoder->state_size <= (int)sizeof((tc)->state) ? \
     (tc)->state.ary : \
     (tc)->state.ptr)

typedef struct {
    struct rb_transcoding *tc;
    unsigned char *out_buf_start;
    unsigned char *out_data_start;
    unsigned char *out_data_end;
    unsigned char *out_buf_end;
    rb_econv_result_t last_result;
} rb_econv_elem_t;

struct rb_econv_t {
    int flags;
    const char *source_encoding_name;
    const char *destination_encoding_name;

    int started;

    const unsigned char *replacement_str;
    size_t replacement_len;
    const char *replacement_enc;
    int replacement_allocated;

    unsigned char *in_buf_start;
    unsigned char *in_data_start;
    unsigned char *in_data_end;
    unsigned char *in_buf_end;
    rb_econv_elem_t *elems;
    int num_allocated;
    int num_trans;
    int num_finished;
    struct rb_transcoding *last_tc;

    /* last error */
    struct {
        rb_econv_result_t result;
        struct rb_transcoding *error_tc;
        const char *source_encoding;
        const char *destination_encoding;
        const unsigned char *error_bytes_start;
        size_t error_bytes_len;
        size_t readagain_len;
    } last_error;

    /* The following fields are only for Encoding::Converter.
     * rb_econv_open set them NULL. */
    rb_encoding *source_encoding;
    rb_encoding *destination_encoding;
};

/*
 *  Dispatch data and logic
 */

#define DECORATOR_P(sname, dname) (*(sname) == '\0')

typedef struct {
    const char *sname;
    const char *dname;
    const char *lib; /* null means means no need to load a library */
    const rb_transcoder *transcoder;
} transcoder_entry_t;

static st_table *transcoder_table;

static transcoder_entry_t *
make_transcoder_entry(const char *sname, const char *dname)
{
    st_data_t val;
    st_table *table2;

    if (!st_lookup(transcoder_table, (st_data_t)sname, &val)) {
        val = (st_data_t)st_init_strcasetable();
        st_add_direct(transcoder_table, (st_data_t)sname, val);
    }
    table2 = (st_table *)val;
    if (!st_lookup(table2, (st_data_t)dname, &val)) {
        transcoder_entry_t *entry = ALLOC(transcoder_entry_t);
        entry->sname = sname;
        entry->dname = dname;
        entry->lib = NULL;
        entry->transcoder = NULL;
        val = (st_data_t)entry;
        st_add_direct(table2, (st_data_t)dname, val);
    }
    return (transcoder_entry_t *)val;
}

static transcoder_entry_t *
get_transcoder_entry(const char *sname, const char *dname)
{
    st_data_t val;
    st_table *table2;

    if (!st_lookup(transcoder_table, (st_data_t)sname, &val)) {
        return NULL;
    }
    table2 = (st_table *)val;
    if (!st_lookup(table2, (st_data_t)dname, &val)) {
        return NULL;
    }
    return (transcoder_entry_t *)val;
}

void
rb_register_transcoder(const rb_transcoder *tr)
{
    const char *const sname = tr->src_encoding;
    const char *const dname = tr->dst_encoding;

    transcoder_entry_t *entry;

    entry = make_transcoder_entry(sname, dname);
    if (entry->transcoder) {
	rb_raise(rb_eArgError, "transcoder from %s to %s has been already registered",
		 sname, dname);
    }

    entry->transcoder = tr;
}

static void
declare_transcoder(const char *sname, const char *dname, const char *lib)
{
    transcoder_entry_t *entry;

    entry = make_transcoder_entry(sname, dname);
    entry->lib = lib;
}

static const char transcoder_lib_prefix[] = "enc/trans/";

void
rb_declare_transcoder(const char *enc1, const char *enc2, const char *lib)
{
    if (!lib) {
	rb_raise(rb_eArgError, "invalid library name - (null)");
    }
    declare_transcoder(enc1, enc2, lib);
}

#define encoding_equal(enc1, enc2) (STRCASECMP((enc1), (enc2)) == 0)

typedef struct search_path_queue_tag {
    struct search_path_queue_tag *next;
    const char *enc;
} search_path_queue_t;

typedef struct {
    st_table *visited;
    search_path_queue_t *queue;
    search_path_queue_t **queue_last_ptr;
    const char *base_enc;
} search_path_bfs_t;

static int
transcode_search_path_i(st_data_t key, st_data_t val, st_data_t arg)
{
    const char *dname = (const char *)key;
    search_path_bfs_t *bfs = (search_path_bfs_t *)arg;
    search_path_queue_t *q;

    if (st_lookup(bfs->visited, (st_data_t)dname, &val)) {
        return ST_CONTINUE;
    }

    q = ALLOC(search_path_queue_t);
    q->enc = dname;
    q->next = NULL;
    *bfs->queue_last_ptr = q;
    bfs->queue_last_ptr = &q->next;

    st_add_direct(bfs->visited, (st_data_t)dname, (st_data_t)bfs->base_enc);
    return ST_CONTINUE;
}

static int
transcode_search_path(const char *sname, const char *dname,
    void (*callback)(const char *sname, const char *dname, int depth, void *arg),
    void *arg)
{
    search_path_bfs_t bfs;
    search_path_queue_t *q;
    st_data_t val;
    st_table *table2;
    int found;
    int pathlen = -1;

    if (encoding_equal(sname, dname))
        return -1;

    q = ALLOC(search_path_queue_t);
    q->enc = sname;
    q->next = NULL;
    bfs.queue_last_ptr = &q->next;
    bfs.queue = q;

    bfs.visited = st_init_strcasetable();
    st_add_direct(bfs.visited, (st_data_t)sname, (st_data_t)NULL);

    while (bfs.queue) {
        q = bfs.queue;
        bfs.queue = q->next;
        if (!bfs.queue)
            bfs.queue_last_ptr = &bfs.queue;

        if (!st_lookup(transcoder_table, (st_data_t)q->enc, &val)) {
            xfree(q);
            continue;
        }
        table2 = (st_table *)val;

        if (st_lookup(table2, (st_data_t)dname, &val)) {
            st_add_direct(bfs.visited, (st_data_t)dname, (st_data_t)q->enc);
            xfree(q);
            found = 1;
            goto cleanup;
        }

        bfs.base_enc = q->enc;
        st_foreach(table2, transcode_search_path_i, (st_data_t)&bfs);
        bfs.base_enc = NULL;

        xfree(q);
    }
    found = 0;

  cleanup:
    while (bfs.queue) {
        q = bfs.queue;
        bfs.queue = q->next;
        xfree(q);
    }

    if (found) {
        const char *enc = dname;
        int depth;
        pathlen = 0;
        while (1) {
            st_lookup(bfs.visited, (st_data_t)enc, &val);
            if (!val)
                break;
            pathlen++;
            enc = (const char *)val;
        }
        depth = pathlen;
        enc = dname;
        while (1) {
            st_lookup(bfs.visited, (st_data_t)enc, &val);
            if (!val)
                break;
            callback((const char *)val, enc, --depth, arg);
            enc = (const char *)val;
        }
    }

    st_free_table(bfs.visited);

    return pathlen; /* is -1 if not found */
}

static const rb_transcoder *
load_transcoder_entry(transcoder_entry_t *entry)
{
    if (entry->transcoder)
        return entry->transcoder;

    if (entry->lib) {
        const char *const lib = entry->lib;
        const size_t len = strlen(lib);
        const size_t total_len = sizeof(transcoder_lib_prefix) - 1 + len;
        const VALUE fn = rb_str_new(0, total_len);
        char *const path = RSTRING_PTR(fn);
	const int safe = rb_safe_level();

        entry->lib = NULL;

        memcpy(path, transcoder_lib_prefix, sizeof(transcoder_lib_prefix) - 1);
        memcpy(path + sizeof(transcoder_lib_prefix) - 1, lib, len);
        rb_str_set_len(fn, total_len);
        FL_UNSET(fn, FL_TAINT|FL_UNTRUSTED);
        OBJ_FREEZE(fn);
        if (!rb_require_safe(fn, safe > 3 ? 3 : safe))
            return NULL;
    }

    if (entry->transcoder)
        return entry->transcoder;

    return NULL;
}

static const char*
get_replacement_character(const char *encname, size_t *len_ret, const char **repl_encname_ptr)
{
    if (encoding_equal(encname, "UTF-8")) {
        *len_ret = 3;
        *repl_encname_ptr = "UTF-8";
        return "\xEF\xBF\xBD";
    }
    else {
        *len_ret = 1;
        *repl_encname_ptr = "US-ASCII";
        return "?";
    }
}

/*
 *  Transcoding engine logic
 */

static const unsigned char *
transcode_char_start(rb_transcoding *tc,
                         const unsigned char *in_start,
                         const unsigned char *inchar_start,
                         const unsigned char *in_p,
                         size_t *char_len_ptr)
{
    const unsigned char *ptr;
    if (inchar_start - in_start < tc->recognized_len) {
        MEMCPY(TRANSCODING_READBUF(tc) + tc->recognized_len,
               inchar_start, unsigned char, in_p - inchar_start);
        ptr = TRANSCODING_READBUF(tc);
    }
    else {
        ptr = inchar_start - tc->recognized_len;
    }
    *char_len_ptr = tc->recognized_len + (in_p - inchar_start);
    return ptr;
}

static rb_econv_result_t
transcode_restartable0(const unsigned char **in_pos, unsigned char **out_pos,
                      const unsigned char *in_stop, unsigned char *out_stop,
                      rb_transcoding *tc,
                      const int opt)
{
    const rb_transcoder *tr = tc->transcoder;
    int unitlen = tr->input_unit_length;
    ssize_t readagain_len = 0;

    const unsigned char *inchar_start;
    const unsigned char *in_p;

    unsigned char *out_p;

    in_p = inchar_start = *in_pos;

    out_p = *out_pos;

#define SUSPEND(ret, num) \
    do { \
        tc->resume_position = (num); \
        if (0 < in_p - inchar_start) \
            MEMMOVE(TRANSCODING_READBUF(tc)+tc->recognized_len, \
                   inchar_start, unsigned char, in_p - inchar_start); \
        *in_pos = in_p; \
        *out_pos = out_p; \
        tc->recognized_len += in_p - inchar_start; \
        if (readagain_len) { \
            tc->recognized_len -= readagain_len; \
            tc->readagain_len = readagain_len; \
        } \
        return (ret); \
        resume_label ## num:; \
    } while (0)
#define SUSPEND_OBUF(num) \
    do { \
        while (out_stop - out_p < 1) { SUSPEND(econv_destination_buffer_full, num); } \
    } while (0)

#define SUSPEND_AFTER_OUTPUT(num) \
    if ((opt & ECONV_AFTER_OUTPUT) && *out_pos != out_p) { \
        SUSPEND(econv_after_output, num); \
    }

#define next_table (tc->next_table)
#define next_info (tc->next_info)
#define next_byte (tc->next_byte)
#define writebuf_len (tc->writebuf_len)
#define writebuf_off (tc->writebuf_off)

    switch (tc->resume_position) {
      case 0: break;
      case 1: goto resume_label1;
      case 2: goto resume_label2;
      case 3: goto resume_label3;
      case 4: goto resume_label4;
      case 5: goto resume_label5;
      case 6: goto resume_label6;
      case 7: goto resume_label7;
      case 8: goto resume_label8;
      case 9: goto resume_label9;
      case 10: goto resume_label10;
      case 11: goto resume_label11;
      case 12: goto resume_label12;
      case 13: goto resume_label13;
      case 14: goto resume_label14;
      case 15: goto resume_label15;
      case 16: goto resume_label16;
      case 17: goto resume_label17;
      case 18: goto resume_label18;
      case 19: goto resume_label19;
      case 20: goto resume_label20;
      case 21: goto resume_label21;
      case 22: goto resume_label22;
      case 23: goto resume_label23;
      case 24: goto resume_label24;
      case 25: goto resume_label25;
      case 26: goto resume_label26;
      case 27: goto resume_label27;
      case 28: goto resume_label28;
      case 29: goto resume_label29;
      case 30: goto resume_label30;
      case 31: goto resume_label31;
      case 32: goto resume_label32;
      case 33: goto resume_label33;
      case 34: goto resume_label34;
    }

    while (1) {
        inchar_start = in_p;
        tc->recognized_len = 0;
	next_table = tr->conv_tree_start;

        SUSPEND_AFTER_OUTPUT(24);

        if (in_stop <= in_p) {
            if (!(opt & ECONV_PARTIAL_INPUT))
                break;
            SUSPEND(econv_source_buffer_empty, 7);
            continue;
        }

#define BYTE_ADDR(index) (tr->byte_array + (index))
#define WORD_ADDR(index) (tr->word_array + INFO2WORDINDEX(index))
#define BL_BASE BYTE_ADDR(BYTE_LOOKUP_BASE(WORD_ADDR(next_table)))
#define BL_INFO WORD_ADDR(BYTE_LOOKUP_INFO(WORD_ADDR(next_table)))
#define BL_MIN_BYTE     (BL_BASE[0])
#define BL_MAX_BYTE     (BL_BASE[1])
#define BL_OFFSET(byte) (BL_BASE[2+(byte)-BL_MIN_BYTE])
#define BL_ACTION(byte) (BL_INFO[BL_OFFSET((byte))])

	next_byte = (unsigned char)*in_p++;
      follow_byte:
        if (next_byte < BL_MIN_BYTE || BL_MAX_BYTE < next_byte)
            next_info = INVALID;
        else {
            next_info = (VALUE)BL_ACTION(next_byte);
        }
      follow_info:
	switch (next_info & 0x1F) {
	  case NOMAP:
            {
                const unsigned char *p = inchar_start;
                writebuf_off = 0;
                while (p < in_p) {
                    TRANSCODING_WRITEBUF(tc)[writebuf_off++] = (unsigned char)*p++;
                }
                writebuf_len = writebuf_off;
                writebuf_off = 0;
                while (writebuf_off < writebuf_len) {
                    SUSPEND_OBUF(3);
                    *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                }
            }
            continue;
	  case 0x00: case 0x04: case 0x08: case 0x0C:
	  case 0x10: case 0x14: case 0x18: case 0x1C:
            SUSPEND_AFTER_OUTPUT(25);
	    while (in_p >= in_stop) {
                if (!(opt & ECONV_PARTIAL_INPUT))
                    goto incomplete;
                SUSPEND(econv_source_buffer_empty, 5);
	    }
	    next_byte = (unsigned char)*in_p++;
	    next_table = (unsigned int)next_info;
	    goto follow_byte;
	  case ZERObt: /* drop input */
	    continue;
	  case ONEbt:
            SUSPEND_OBUF(9); *out_p++ = getBT1(next_info);
	    continue;
	  case TWObt:
            SUSPEND_OBUF(10); *out_p++ = getBT1(next_info);
            SUSPEND_OBUF(21); *out_p++ = getBT2(next_info);
	    continue;
	  case THREEbt:
            SUSPEND_OBUF(11); *out_p++ = getBT1(next_info);
            SUSPEND_OBUF(15); *out_p++ = getBT2(next_info);
            SUSPEND_OBUF(16); *out_p++ = getBT3(next_info);
	    continue;
	  case FOURbt:
            SUSPEND_OBUF(12); *out_p++ = getBT0(next_info);
            SUSPEND_OBUF(17); *out_p++ = getBT1(next_info);
            SUSPEND_OBUF(18); *out_p++ = getBT2(next_info);
            SUSPEND_OBUF(19); *out_p++ = getBT3(next_info);
	    continue;
	  case GB4bt:
            SUSPEND_OBUF(29); *out_p++ = getGB4bt0(next_info);
            SUSPEND_OBUF(30); *out_p++ = getGB4bt1(next_info);
            SUSPEND_OBUF(31); *out_p++ = getGB4bt2(next_info);
            SUSPEND_OBUF(32); *out_p++ = getGB4bt3(next_info);
	    continue;
          case STR1:
            tc->output_index = 0;
            while (tc->output_index < STR1_LENGTH(BYTE_ADDR(STR1_BYTEINDEX(next_info)))) {
                SUSPEND_OBUF(28); *out_p++ = BYTE_ADDR(STR1_BYTEINDEX(next_info))[1+tc->output_index];
                tc->output_index++;
            }
            continue;
	  case FUNii:
	    next_info = (VALUE)(*tr->func_ii)(TRANSCODING_STATE(tc), next_info);
	    goto follow_info;
	  case FUNsi:
            {
                const unsigned char *char_start;
                size_t char_len;
                char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                next_info = (VALUE)(*tr->func_si)(TRANSCODING_STATE(tc), char_start, (size_t)char_len);
                goto follow_info;
            }
	  case FUNio:
            SUSPEND_OBUF(13);
            if (tr->max_output <= out_stop - out_p)
                out_p += tr->func_io(TRANSCODING_STATE(tc),
                    next_info, out_p, out_stop - out_p);
            else {
                writebuf_len = tr->func_io(TRANSCODING_STATE(tc),
                    next_info,
                    TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
                writebuf_off = 0;
                while (writebuf_off < writebuf_len) {
                    SUSPEND_OBUF(20);
                    *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                }
            }
	    break;
	  case FUNso:
            {
                const unsigned char *char_start;
                size_t char_len;
                SUSPEND_OBUF(14);
                if (tr->max_output <= out_stop - out_p) {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    out_p += tr->func_so(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len,
                        out_p, out_stop - out_p);
                }
                else {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    writebuf_len = tr->func_so(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len,
                        TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
                    writebuf_off = 0;
                    while (writebuf_off < writebuf_len) {
                        SUSPEND_OBUF(22);
                        *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                    }
                }
                break;
            }
      case FUNsio:
            {
                const unsigned char *char_start;
                size_t char_len;
                SUSPEND_OBUF(33);
                if (tr->max_output <= out_stop - out_p) {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    out_p += tr->func_sio(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len, next_info,
                        out_p, out_stop - out_p);
                }
                else {
                    char_start = transcode_char_start(tc, *in_pos, inchar_start, in_p, &char_len);
                    writebuf_len = tr->func_sio(TRANSCODING_STATE(tc),
                        char_start, (size_t)char_len, next_info,
                        TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
                    writebuf_off = 0;
                    while (writebuf_off < writebuf_len) {
                        SUSPEND_OBUF(34);
                        *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
                    }
                }
                break;
            }
	  case INVALID:
            if (tc->recognized_len + (in_p - inchar_start) <= unitlen) {
                if (tc->recognized_len + (in_p - inchar_start) < unitlen)
                    SUSPEND_AFTER_OUTPUT(26);
                while ((opt & ECONV_PARTIAL_INPUT) && tc->recognized_len + (in_stop - inchar_start) < unitlen) {
                    in_p = in_stop;
                    SUSPEND(econv_source_buffer_empty, 8);
                }
                if (tc->recognized_len + (in_stop - inchar_start) <= unitlen) {
                    in_p = in_stop;
                }
                else {
                    in_p = inchar_start + (unitlen - tc->recognized_len);
                }
            }
            else {
                ssize_t invalid_len; /* including the last byte which causes invalid */
                ssize_t discard_len;
                invalid_len = tc->recognized_len + (in_p - inchar_start);
                discard_len = ((invalid_len - 1) / unitlen) * unitlen;
                readagain_len = invalid_len - discard_len;
            }
            goto invalid;
	  case UNDEF:
	    goto undef;
	  default:
	    rb_raise(rb_eRuntimeError, "unknown transcoding instruction");
	}
	continue;

      invalid:
        SUSPEND(econv_invalid_byte_sequence, 1);
        continue;

      incomplete:
        SUSPEND(econv_incomplete_input, 27);
        continue;

      undef:
        SUSPEND(econv_undefined_conversion, 2);
        continue;
    }

    /* cleanup */
    if (tr->finish_func) {
        SUSPEND_OBUF(4);
        if (tr->max_output <= out_stop - out_p) {
            out_p += tr->finish_func(TRANSCODING_STATE(tc),
                out_p, out_stop - out_p);
        }
        else {
            writebuf_len = tr->finish_func(TRANSCODING_STATE(tc),
                TRANSCODING_WRITEBUF(tc), TRANSCODING_WRITEBUF_SIZE(tc));
            writebuf_off = 0;
            while (writebuf_off < writebuf_len) {
                SUSPEND_OBUF(23);
                *out_p++ = TRANSCODING_WRITEBUF(tc)[writebuf_off++];
            }
        }
    }
    while (1)
        SUSPEND(econv_finished, 6);
#undef SUSPEND
#undef next_table
#undef next_info
#undef next_byte
#undef writebuf_len
#undef writebuf_off
}

static rb_econv_result_t
transcode_restartable(const unsigned char **in_pos, unsigned char **out_pos,
                      const unsigned char *in_stop, unsigned char *out_stop,
                      rb_transcoding *tc,
                      const int opt)
{
    if (tc->readagain_len) {
        unsigned char *readagain_buf = ALLOCA_N(unsigned char, tc->readagain_len);
        const unsigned char *readagain_pos = readagain_buf;
        const unsigned char *readagain_stop = readagain_buf + tc->readagain_len;
        rb_econv_result_t res;

        MEMCPY(readagain_buf, TRANSCODING_READBUF(tc) + tc->recognized_len,
               unsigned char, tc->readagain_len);
        tc->readagain_len = 0;
        res = transcode_restartable0(&readagain_pos, out_pos, readagain_stop, out_stop, tc, opt|ECONV_PARTIAL_INPUT);
        if (res != econv_source_buffer_empty) {
            MEMCPY(TRANSCODING_READBUF(tc) + tc->recognized_len + tc->readagain_len,
                   readagain_pos, unsigned char, readagain_stop - readagain_pos);
            tc->readagain_len += readagain_stop - readagain_pos;
            return res;
        }
    }
    return transcode_restartable0(in_pos, out_pos, in_stop, out_stop, tc, opt);
}

static rb_transcoding *
rb_transcoding_open_by_transcoder(const rb_transcoder *tr, int flags)
{
    rb_transcoding *tc;

    tc = ALLOC(rb_transcoding);
    tc->transcoder = tr;
    tc->flags = flags;
    if (TRANSCODING_STATE_EMBED_MAX < tr->state_size)
        tc->state.ptr = xmalloc(tr->state_size);
    if (tr->state_init_func) {
        (tr->state_init_func)(TRANSCODING_STATE(tc)); /* xxx: check return value */
    }
    tc->resume_position = 0;
    tc->recognized_len = 0;
    tc->readagain_len = 0;
    tc->writebuf_len = 0;
    tc->writebuf_off = 0;
    if ((int)sizeof(tc->readbuf.ary) < tr->max_input) {
        tc->readbuf.ptr = xmalloc(tr->max_input);
    }
    if ((int)sizeof(tc->writebuf.ary) < tr->max_output) {
        tc->writebuf.ptr = xmalloc(tr->max_output);
    }
    return tc;
}

static rb_econv_result_t
rb_transcoding_convert(rb_transcoding *tc,
  const unsigned char **input_ptr, const unsigned char *input_stop,
  unsigned char **output_ptr, unsigned char *output_stop,
  int flags)
{
    return transcode_restartable(
                input_ptr, output_ptr,
                input_stop, output_stop,
                tc, flags);
}

static void
rb_transcoding_close(rb_transcoding *tc)
{
    const rb_transcoder *tr = tc->transcoder;
    if (tr->state_fini_func) {
        (tr->state_fini_func)(TRANSCODING_STATE(tc)); /* check return value? */
    }
    if (TRANSCODING_STATE_EMBED_MAX < tr->state_size)
        xfree(tc->state.ptr);
    if ((int)sizeof(tc->readbuf.ary) < tr->max_input)
        xfree(tc->readbuf.ptr);
    if ((int)sizeof(tc->writebuf.ary) < tr->max_output)
        xfree(tc->writebuf.ptr);
    xfree(tc);
}

static size_t
rb_transcoding_memsize(rb_transcoding *tc)
{
    size_t size = sizeof(rb_transcoding);
    const rb_transcoder *tr = tc->transcoder;

    if (TRANSCODING_STATE_EMBED_MAX < tr->state_size) {
	size += tr->state_size;
    }
    if ((int)sizeof(tc->readbuf.ary) < tr->max_input) {
	size += tr->max_input;
    }
    if ((int)sizeof(tc->writebuf.ary) < tr->max_output) {
	size += tr->max_output;
    }
    return size;
}

static rb_econv_t *
rb_econv_alloc(int n_hint)
{
    rb_econv_t *ec;

    if (n_hint <= 0)
        n_hint = 1;

    ec = ALLOC(rb_econv_t);
    ec->flags = 0;
    ec->source_encoding_name = NULL;
    ec->destination_encoding_name = NULL;
    ec->started = 0;
    ec->replacement_str = NULL;
    ec->replacement_len = 0;
    ec->replacement_enc = NULL;
    ec->replacement_allocated = 0;
    ec->in_buf_start = NULL;
    ec->in_data_start = NULL;
    ec->in_data_end = NULL;
    ec->in_buf_end = NULL;
    ec->num_allocated = n_hint;
    ec->num_trans = 0;
    ec->elems = ALLOC_N(rb_econv_elem_t, ec->num_allocated);
    ec->num_finished = 0;
    ec->last_tc = NULL;
    ec->last_error.result = econv_source_buffer_empty;
    ec->last_error.error_tc = NULL;
    ec->last_error.source_encoding = NULL;
    ec->last_error.destination_encoding = NULL;
    ec->last_error.error_bytes_start = NULL;
    ec->last_error.error_bytes_len = 0;
    ec->last_error.readagain_len = 0;
    ec->source_encoding = NULL;
    ec->destination_encoding = NULL;
    return ec;
}

static int
rb_econv_add_transcoder_at(rb_econv_t *ec, const rb_transcoder *tr, int i)
{
    int n, j;
    int bufsize = 4096;
    unsigned char *p;

    if (ec->num_trans == ec->num_allocated) {
        n = ec->num_allocated * 2;
        REALLOC_N(ec->elems, rb_econv_elem_t, n);
        ec->num_allocated = n;
    }

    p = xmalloc(bufsize);

    MEMMOVE(ec->elems+i+1, ec->elems+i, rb_econv_elem_t, ec->num_trans-i);

    ec->elems[i].tc = rb_transcoding_open_by_transcoder(tr, 0);
    ec->elems[i].out_buf_start = p;
    ec->elems[i].out_buf_end = p + bufsize;
    ec->elems[i].out_data_start = p;
    ec->elems[i].out_data_end = p;
    ec->elems[i].last_result = econv_source_buffer_empty;

    ec->num_trans++;

    if (!DECORATOR_P(tr->src_encoding, tr->dst_encoding))
        for (j = ec->num_trans-1; i <= j; j--) {
            rb_transcoding *tc = ec->elems[j].tc;
            const rb_transcoder *tr2 = tc->transcoder;
            if (!DECORATOR_P(tr2->src_encoding, tr2->dst_encoding)) {
                ec->last_tc = tc;
                break;
            }
        }

    return 0;
}

static rb_econv_t *
rb_econv_open_by_transcoder_entries(int n, transcoder_entry_t **entries)
{
    rb_econv_t *ec;
    int i, ret;

    for (i = 0; i < n; i++) {
        const rb_transcoder *tr;
        tr = load_transcoder_entry(entries[i]);
        if (!tr)
            return NULL;
    }

    ec = rb_econv_alloc(n);

    for (i = 0; i < n; i++) {
        const rb_transcoder *tr = load_transcoder_entry(entries[i]);
        ret = rb_econv_add_transcoder_at(ec, tr, ec->num_trans);
        if (ret == -1) {
            rb_econv_close(ec);
            return NULL;
        }
    }

    return ec;
}

struct trans_open_t {
    transcoder_entry_t **entries;
    int num_additional;
};

static void
trans_open_i(const char *sname, const char *dname, int depth, void *arg)
{
    struct trans_open_t *toarg = arg;

    if (!toarg->entries) {
        toarg->entries = ALLOC_N(transcoder_entry_t *, depth+1+toarg->num_additional);
    }
    toarg->entries[depth] = get_transcoder_entry(sname, dname);
}

static rb_econv_t *
rb_econv_open0(const char *sname, const char *dname, int ecflags)
{
    transcoder_entry_t **entries = NULL;
    int num_trans;
    rb_econv_t *ec;

    int sidx, didx;

    if (*sname) {
        sidx = rb_enc_find_index(sname);
        if (0 <= sidx) {
            rb_enc_from_index(sidx);
        }
    }

    if (*dname) {
        didx = rb_enc_find_index(dname);
        if (0 <= didx) {
            rb_enc_from_index(didx);
        }
    }

    if (*sname == '\0' && *dname == '\0') {
        num_trans = 0;
        entries = NULL;
    }
    else {
        struct trans_open_t toarg;
        toarg.entries = NULL;
        toarg.num_additional = 0;
        num_trans = transcode_search_path(sname, dname, trans_open_i, (void *)&toarg);
        entries = toarg.entries;
        if (num_trans < 0) {
            xfree(entries);
            return NULL;
        }
    }

    ec = rb_econv_open_by_transcoder_entries(num_trans, entries);
    xfree(entries);
    if (!ec)
        return NULL;

    ec->flags = ecflags;
    ec->source_encoding_name = sname;
    ec->destination_encoding_name = dname;

    return ec;
}

#define MAX_ECFLAGS_DECORATORS 32

static int
decorator_names(int ecflags, const char **decorators_ret)
{
    int num_decorators;

    switch (ecflags & ECONV_NEWLINE_DECORATOR_MASK) {
      case ECONV_UNIVERSAL_NEWLINE_DECORATOR:
      case ECONV_CRLF_NEWLINE_DECORATOR:
      case ECONV_CR_NEWLINE_DECORATOR:
      case 0:
	break;
      default:
        return -1;
    }

    if ((ecflags & ECONV_XML_TEXT_DECORATOR) &&
        (ecflags & ECONV_XML_ATTR_CONTENT_DECORATOR))
        return -1;

    num_decorators = 0;

    if (ecflags & ECONV_XML_TEXT_DECORATOR)
        decorators_ret[num_decorators++] = "xml_text_escape";
    if (ecflags & ECONV_XML_ATTR_CONTENT_DECORATOR)
        decorators_ret[num_decorators++] = "xml_attr_content_escape";
    if (ecflags & ECONV_XML_ATTR_QUOTE_DECORATOR)
        decorators_ret[num_decorators++] = "xml_attr_quote";

    if (ecflags & ECONV_CRLF_NEWLINE_DECORATOR)
        decorators_ret[num_decorators++] = "crlf_newline";
    if (ecflags & ECONV_CR_NEWLINE_DECORATOR)
        decorators_ret[num_decorators++] = "cr_newline";
    if (ecflags & ECONV_UNIVERSAL_NEWLINE_DECORATOR)
        decorators_ret[num_decorators++] = "universal_newline";

    return num_decorators;
}

rb_econv_t *
rb_econv_open(const char *sname, const char *dname, int ecflags)
{
    rb_econv_t *ec;
    int num_decorators;
    const char *decorators[MAX_ECFLAGS_DECORATORS];
    int i;

    num_decorators = decorator_names(ecflags, decorators);
    if (num_decorators == -1)
        return NULL;

    ec = rb_econv_open0(sname, dname, ecflags & ECONV_ERROR_HANDLER_MASK);
    if (!ec)
        return NULL;

    for (i = 0; i < num_decorators; i++)
        if (rb_econv_decorate_at_last(ec, decorators[i]) == -1) {
            rb_econv_close(ec);
            return NULL;
        }

    ec->flags |= ecflags & ~ECONV_ERROR_HANDLER_MASK;

    return ec;
}

static int
trans_sweep(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags,
    int start)
{
    int try;
    int i, f;

    const unsigned char **ipp, *is, *iold;
    unsigned char **opp, *os, *oold;
    rb_econv_result_t res;

    try = 1;
    while (try) {
        try = 0;
        for (i = start; i < ec->num_trans; i++) {
            rb_econv_elem_t *te = &ec->elems[i];

            if (i == 0) {
                ipp = input_ptr;
                is = input_stop;
            }
            else {
                rb_econv_elem_t *prev_te = &ec->elems[i-1];
                ipp = (const unsigned char **)&prev_te->out_data_start;
                is = prev_te->out_data_end;
            }

            if (i == ec->num_trans-1) {
                opp = output_ptr;
                os = output_stop;
            }
            else {
                if (te->out_buf_start != te->out_data_start) {
                    ssize_t len = te->out_data_end - te->out_data_start;
                    ssize_t off = te->out_data_start - te->out_buf_start;
                    MEMMOVE(te->out_buf_start, te->out_data_start, unsigned char, len);
                    te->out_data_start = te->out_buf_start;
                    te->out_data_end -= off;
                }
                opp = &te->out_data_end;
                os = te->out_buf_end;
            }

            f = flags;
            if (ec->num_finished != i)
                f |= ECONV_PARTIAL_INPUT;
            if (i == 0 && (flags & ECONV_AFTER_OUTPUT)) {
                start = 1;
                flags &= ~ECONV_AFTER_OUTPUT;
            }
            if (i != 0)
                f &= ~ECONV_AFTER_OUTPUT;
            iold = *ipp;
            oold = *opp;
            te->last_result = res = rb_transcoding_convert(te->tc, ipp, is, opp, os, f);
            if (iold != *ipp || oold != *opp)
                try = 1;

            switch (res) {
              case econv_invalid_byte_sequence:
              case econv_incomplete_input:
              case econv_undefined_conversion:
              case econv_after_output:
                return i;

              case econv_destination_buffer_full:
              case econv_source_buffer_empty:
                break;

              case econv_finished:
                ec->num_finished = i+1;
                break;
            }
        }
    }
    return -1;
}

static rb_econv_result_t
rb_trans_conv(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags,
    int *result_position_ptr)
{
    int i;
    int needreport_index;
    int sweep_start;

    unsigned char empty_buf;
    unsigned char *empty_ptr = &empty_buf;

    if (!input_ptr) {
        input_ptr = (const unsigned char **)&empty_ptr;
        input_stop = empty_ptr;
    }

    if (!output_ptr) {
        output_ptr = &empty_ptr;
        output_stop = empty_ptr;
    }

    if (ec->elems[0].last_result == econv_after_output)
        ec->elems[0].last_result = econv_source_buffer_empty;

    needreport_index = -1;
    for (i = ec->num_trans-1; 0 <= i; i--) {
        switch (ec->elems[i].last_result) {
          case econv_invalid_byte_sequence:
          case econv_incomplete_input:
          case econv_undefined_conversion:
          case econv_after_output:
          case econv_finished:
            sweep_start = i+1;
            needreport_index = i;
            goto found_needreport;

          case econv_destination_buffer_full:
          case econv_source_buffer_empty:
            break;

          default:
            rb_bug("unexpected transcode last result");
        }
    }

    /* /^[sd]+$/ is confirmed.  but actually /^s*d*$/. */

    if (ec->elems[ec->num_trans-1].last_result == econv_destination_buffer_full &&
        (flags & ECONV_AFTER_OUTPUT)) {
        rb_econv_result_t res;

        res = rb_trans_conv(ec, NULL, NULL, output_ptr, output_stop,
                (flags & ~ECONV_AFTER_OUTPUT)|ECONV_PARTIAL_INPUT,
                result_position_ptr);

        if (res == econv_source_buffer_empty)
            return econv_after_output;
        return res;
    }

    sweep_start = 0;

  found_needreport:

    do {
        needreport_index = trans_sweep(ec, input_ptr, input_stop, output_ptr, output_stop, flags, sweep_start);
        sweep_start = needreport_index + 1;
    } while (needreport_index != -1 && needreport_index != ec->num_trans-1);

    for (i = ec->num_trans-1; 0 <= i; i--) {
        if (ec->elems[i].last_result != econv_source_buffer_empty) {
            rb_econv_result_t res = ec->elems[i].last_result;
            if (res == econv_invalid_byte_sequence ||
                res == econv_incomplete_input ||
                res == econv_undefined_conversion ||
                res == econv_after_output) {
                ec->elems[i].last_result = econv_source_buffer_empty;
            }
            if (result_position_ptr)
                *result_position_ptr = i;
            return res;
        }
    }
    if (result_position_ptr)
        *result_position_ptr = -1;
    return econv_source_buffer_empty;
}

static rb_econv_result_t
rb_econv_convert0(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags)
{
    rb_econv_result_t res;
    int result_position;
    int has_output = 0;

    memset(&ec->last_error, 0, sizeof(ec->last_error));

    if (ec->num_trans == 0) {
        size_t len;
        if (ec->in_buf_start && ec->in_data_start != ec->in_data_end) {
            if (output_stop - *output_ptr < ec->in_data_end - ec->in_data_start) {
                len = output_stop - *output_ptr;
                memcpy(*output_ptr, ec->in_data_start, len);
                *output_ptr = output_stop;
                ec->in_data_start += len;
                res = econv_destination_buffer_full;
                goto gotresult;
            }
            len = ec->in_data_end - ec->in_data_start;
            memcpy(*output_ptr, ec->in_data_start, len);
            *output_ptr += len;
            ec->in_data_start = ec->in_data_end = ec->in_buf_start;
            if (flags & ECONV_AFTER_OUTPUT) {
                res = econv_after_output;
                goto gotresult;
            }
        }
        if (output_stop - *output_ptr < input_stop - *input_ptr) {
            len = output_stop - *output_ptr;
        }
        else {
            len = input_stop - *input_ptr;
        }
        if (0 < len && (flags & ECONV_AFTER_OUTPUT)) {
            *(*output_ptr)++ = *(*input_ptr)++;
            res = econv_after_output;
            goto gotresult;
        }
        memcpy(*output_ptr, *input_ptr, len);
        *output_ptr += len;
        *input_ptr += len;
        if (*input_ptr != input_stop)
            res = econv_destination_buffer_full;
        else if (flags & ECONV_PARTIAL_INPUT)
            res = econv_source_buffer_empty;
        else
            res = econv_finished;
        goto gotresult;
    }

    if (ec->elems[ec->num_trans-1].out_data_start) {
        unsigned char *data_start = ec->elems[ec->num_trans-1].out_data_start;
        unsigned char *data_end = ec->elems[ec->num_trans-1].out_data_end;
        if (data_start != data_end) {
            size_t len;
            if (output_stop - *output_ptr < data_end - data_start) {
                len = output_stop - *output_ptr;
                memcpy(*output_ptr, data_start, len);
                *output_ptr = output_stop;
                ec->elems[ec->num_trans-1].out_data_start += len;
                res = econv_destination_buffer_full;
                goto gotresult;
            }
            len = data_end - data_start;
            memcpy(*output_ptr, data_start, len);
            *output_ptr += len;
            ec->elems[ec->num_trans-1].out_data_start =
                ec->elems[ec->num_trans-1].out_data_end =
                ec->elems[ec->num_trans-1].out_buf_start;
            has_output = 1;
        }
    }

    if (ec->in_buf_start &&
        ec->in_data_start != ec->in_data_end) {
        res = rb_trans_conv(ec, (const unsigned char **)&ec->in_data_start, ec->in_data_end, output_ptr, output_stop,
                (flags&~ECONV_AFTER_OUTPUT)|ECONV_PARTIAL_INPUT, &result_position);
        if (res != econv_source_buffer_empty)
            goto gotresult;
    }

    if (has_output &&
        (flags & ECONV_AFTER_OUTPUT) &&
        *input_ptr != input_stop) {
        input_stop = *input_ptr;
        res = rb_trans_conv(ec, input_ptr, input_stop, output_ptr, output_stop, flags, &result_position);
        if (res == econv_source_buffer_empty)
            res = econv_after_output;
    }
    else if ((flags & ECONV_AFTER_OUTPUT) ||
        ec->num_trans == 1) {
        res = rb_trans_conv(ec, input_ptr, input_stop, output_ptr, output_stop, flags, &result_position);
    }
    else {
        flags |= ECONV_AFTER_OUTPUT;
        do {
            res = rb_trans_conv(ec, input_ptr, input_stop, output_ptr, output_stop, flags, &result_position);
        } while (res == econv_after_output);
    }

  gotresult:
    ec->last_error.result = res;
    if (res == econv_invalid_byte_sequence ||
        res == econv_incomplete_input ||
        res == econv_undefined_conversion) {
        rb_transcoding *error_tc = ec->elems[result_position].tc;
        ec->last_error.error_tc = error_tc;
        ec->last_error.source_encoding = error_tc->transcoder->src_encoding;
        ec->last_error.destination_encoding = error_tc->transcoder->dst_encoding;
        ec->last_error.error_bytes_start = TRANSCODING_READBUF(error_tc);
        ec->last_error.error_bytes_len = error_tc->recognized_len;
        ec->last_error.readagain_len = error_tc->readagain_len;
    }

    return res;
}

static int output_replacement_character(rb_econv_t *ec);

static int
output_hex_charref(rb_econv_t *ec)
{
    int ret;
    unsigned char utfbuf[1024];
    const unsigned char *utf;
    size_t utf_len;
    int utf_allocated = 0;
    char charef_buf[16];
    const unsigned char *p;

    if (encoding_equal(ec->last_error.source_encoding, "UTF-32BE")) {
        utf = ec->last_error.error_bytes_start;
        utf_len = ec->last_error.error_bytes_len;
    }
    else {
        utf = allocate_converted_string(ec->last_error.source_encoding, "UTF-32BE",
                ec->last_error.error_bytes_start, ec->last_error.error_bytes_len,
                utfbuf, sizeof(utfbuf),
                &utf_len);
        if (!utf)
            return -1;
        if (utf != utfbuf && utf != ec->last_error.error_bytes_start)
            utf_allocated = 1;
    }

    if (utf_len % 4 != 0)
        goto fail;

    p = utf;
    while (4 <= utf_len) {
        unsigned int u = 0;
        u += p[0] << 24;
        u += p[1] << 16;
        u += p[2] << 8;
        u += p[3];
        snprintf(charef_buf, sizeof(charef_buf), "&#x%X;", u);

        ret = rb_econv_insert_output(ec, (unsigned char *)charef_buf, strlen(charef_buf), "US-ASCII");
        if (ret == -1)
            goto fail;

        p += 4;
        utf_len -= 4;
    }

    if (utf_allocated)
        xfree((void *)utf);
    return 0;

  fail:
    if (utf_allocated)
        xfree((void *)utf);
    return -1;
}

rb_econv_result_t
rb_econv_convert(rb_econv_t *ec,
    const unsigned char **input_ptr, const unsigned char *input_stop,
    unsigned char **output_ptr, unsigned char *output_stop,
    int flags)
{
    rb_econv_result_t ret;

    unsigned char empty_buf;
    unsigned char *empty_ptr = &empty_buf;

    ec->started = 1;

    if (!input_ptr) {
        input_ptr = (const unsigned char **)&empty_ptr;
        input_stop = empty_ptr;
    }

    if (!output_ptr) {
        output_ptr = &empty_ptr;
        output_stop = empty_ptr;
    }

  resume:
    ret = rb_econv_convert0(ec, input_ptr, input_stop, output_ptr, output_stop, flags);

    if (ret == econv_invalid_byte_sequence ||
        ret == econv_incomplete_input) {
	/* deal with invalid byte sequence */
	/* todo: add more alternative behaviors */
        switch (ec->flags & ECONV_INVALID_MASK) {
          case ECONV_INVALID_REPLACE:
	    if (output_replacement_character(ec) == 0)
                goto resume;
	}
    }

    if (ret == econv_undefined_conversion) {
	/* valid character in source encoding
	 * but no related character(s) in destination encoding */
	/* todo: add more alternative behaviors */
        switch (ec->flags & ECONV_UNDEF_MASK) {
          case ECONV_UNDEF_REPLACE:
	    if (output_replacement_character(ec) == 0)
                goto resume;
            break;

          case ECONV_UNDEF_HEX_CHARREF:
            if (output_hex_charref(ec) == 0)
                goto resume;
            break;
        }
    }

    return ret;
}

const char *
rb_econv_encoding_to_insert_output(rb_econv_t *ec)
{
    rb_transcoding *tc = ec->last_tc;
    const rb_transcoder *tr;

    if (tc == NULL)
        return "";

    tr = tc->transcoder;

    if (tr->asciicompat_type == asciicompat_encoder)
        return tr->src_encoding;
    return tr->dst_encoding;
}

static unsigned char *
allocate_converted_string(const char *sname, const char *dname,
        const unsigned char *str, size_t len,
        unsigned char *caller_dst_buf, size_t caller_dst_bufsize,
        size_t *dst_len_ptr)
{
    unsigned char *dst_str;
    size_t dst_len;
    size_t dst_bufsize;

    rb_econv_t *ec;
    rb_econv_result_t res;

    const unsigned char *sp;
    unsigned char *dp;

    if (caller_dst_buf)
        dst_bufsize = caller_dst_bufsize;
    else if (len == 0)
        dst_bufsize = 1;
    else
        dst_bufsize = len;

    ec = rb_econv_open(sname, dname, 0);
    if (ec == NULL)
        return NULL;
    if (caller_dst_buf)
        dst_str = caller_dst_buf;
    else
        dst_str = xmalloc(dst_bufsize);
    dst_len = 0;
    sp = str;
    dp = dst_str+dst_len;
    res = rb_econv_convert(ec, &sp, str+len, &dp, dst_str+dst_bufsize, 0);
    dst_len = dp - dst_str;
    while (res == econv_destination_buffer_full) {
        if (SIZE_MAX/2 < dst_bufsize) {
            goto fail;
        }
        dst_bufsize *= 2;
        if (dst_str == caller_dst_buf) {
            unsigned char *tmp;
            tmp = xmalloc(dst_bufsize);
            memcpy(tmp, dst_str, dst_bufsize/2);
            dst_str = tmp;
        }
        else {
            dst_str = xrealloc(dst_str, dst_bufsize);
        }
        dp = dst_str+dst_len;
        res = rb_econv_convert(ec, &sp, str+len, &dp, dst_str+dst_bufsize, 0);
        dst_len = dp - dst_str;
    }
    if (res != econv_finished) {
        goto fail;
    }
    rb_econv_close(ec);
    *dst_len_ptr = dst_len;
    return dst_str;

  fail:
    if (dst_str != caller_dst_buf)
        xfree(dst_str);
    rb_econv_close(ec);
    return NULL;
}

/* result: 0:success -1:failure */
int
rb_econv_insert_output(rb_econv_t *ec,
    const unsigned char *str, size_t len, const char *str_encoding)
{
    const char *insert_encoding = rb_econv_encoding_to_insert_output(ec);
    unsigned char insert_buf[4096];
    const unsigned char *insert_str = NULL;
    size_t insert_len;

    int last_trans_index;
    rb_transcoding *tc;

    unsigned char **buf_start_p;
    unsigned char **data_start_p;
    unsigned char **data_end_p;
    unsigned char **buf_end_p;

    size_t need;

    ec->started = 1;

    if (len == 0)
        return 0;

    if (encoding_equal(insert_encoding, str_encoding)) {
        insert_str = str;
        insert_len = len;
    }
    else {
        insert_str = allocate_converted_string(str_encoding, insert_encoding,
                str, len, insert_buf, sizeof(insert_buf), &insert_len);
        if (insert_str == NULL)
            return -1;
    }

    need = insert_len;

    last_trans_index = ec->num_trans-1;
    if (ec->num_trans == 0) {
        tc = NULL;
        buf_start_p = &ec->in_buf_start;
        data_start_p = &ec->in_data_start;
        data_end_p = &ec->in_data_end;
        buf_end_p = &ec->in_buf_end;
    }
    else if (ec->elems[last_trans_index].tc->transcoder->asciicompat_type == asciicompat_encoder) {
        tc = ec->elems[last_trans_index].tc;
        need += tc->readagain_len;
        if (need < insert_len)
            goto fail;
        if (last_trans_index == 0) {
            buf_start_p = &ec->in_buf_start;
            data_start_p = &ec->in_data_start;
            data_end_p = &ec->in_data_end;
            buf_end_p = &ec->in_buf_end;
        }
        else {
            rb_econv_elem_t *ee = &ec->elems[last_trans_index-1];
            buf_start_p = &ee->out_buf_start;
            data_start_p = &ee->out_data_start;
            data_end_p = &ee->out_data_end;
            buf_end_p = &ee->out_buf_end;
        }
    }
    else {
        rb_econv_elem_t *ee = &ec->elems[last_trans_index];
        buf_start_p = &ee->out_buf_start;
        data_start_p = &ee->out_data_start;
        data_end_p = &ee->out_data_end;
        buf_end_p = &ee->out_buf_end;
        tc = ec->elems[last_trans_index].tc;
    }

    if (*buf_start_p == NULL) {
        unsigned char *buf = xmalloc(need);
        *buf_start_p = buf;
        *data_start_p = buf;
        *data_end_p = buf;
        *buf_end_p = buf+need;
    }
    else if ((size_t)(*buf_end_p - *data_end_p) < need) {
        MEMMOVE(*buf_start_p, *data_start_p, unsigned char, *data_end_p - *data_start_p);
        *data_end_p = *buf_start_p + (*data_end_p - *data_start_p);
        *data_start_p = *buf_start_p;
        if ((size_t)(*buf_end_p - *data_end_p) < need) {
            unsigned char *buf;
            size_t s = (*data_end_p - *buf_start_p) + need;
            if (s < need)
                goto fail;
            buf = xrealloc(*buf_start_p, s);
            *data_start_p = buf;
            *data_end_p = buf + (*data_end_p - *buf_start_p);
            *buf_start_p = buf;
            *buf_end_p = buf + s;
        }
    }

    memcpy(*data_end_p, insert_str, insert_len);
    *data_end_p += insert_len;
    if (tc && tc->transcoder->asciicompat_type == asciicompat_encoder) {
        memcpy(*data_end_p, TRANSCODING_READBUF(tc)+tc->recognized_len, tc->readagain_len);
        *data_end_p += tc->readagain_len;
        tc->readagain_len = 0;
    }

    if (insert_str != str && insert_str != insert_buf)
        xfree((void*)insert_str);
    return 0;

  fail:
    if (insert_str != str && insert_str != insert_buf)
        xfree((void*)insert_str);
    return -1;
}

void
rb_econv_close(rb_econv_t *ec)
{
    int i;

    if (ec->replacement_allocated) {
        xfree((void *)ec->replacement_str);
    }
    for (i = 0; i < ec->num_trans; i++) {
        rb_transcoding_close(ec->elems[i].tc);
        if (ec->elems[i].out_buf_start)
            xfree(ec->elems[i].out_buf_start);
    }
    xfree(ec->in_buf_start);
    xfree(ec->elems);
    xfree(ec);
}

size_t
rb_econv_memsize(rb_econv_t *ec)
{
    size_t size = sizeof(rb_econv_t);
    int i;

    if (ec->replacement_allocated) {
	size += ec->replacement_len;
    }
    for (i = 0; i < ec->num_trans; i++) {
	size += rb_transcoding_memsize(ec->elems[i].tc);

	if (ec->elems[i].out_buf_start) {
            size += ec->elems[i].out_buf_end - ec->elems[i].out_buf_start;
	}
    }
    size += ec->in_buf_end - ec->in_buf_start;
    size += sizeof(rb_econv_elem_t) * ec->num_allocated;

    return size;
}

int
rb_econv_putbackable(rb_econv_t *ec)
{
    if (ec->num_trans == 0)
        return 0;
#if SIZEOF_SIZE_T > SIZEOF_INT
    if (ec->elems[0].tc->readagain_len > INT_MAX) return INT_MAX;
#endif
    return (int)ec->elems[0].tc->readagain_len;
}

void
rb_econv_putback(rb_econv_t *ec, unsigned char *p, int n)
{
    rb_transcoding *tc;
    if (ec->num_trans == 0 || n == 0)
        return;
    tc = ec->elems[0].tc;
    memcpy(p, TRANSCODING_READBUF(tc) + tc->recognized_len + tc->readagain_len - n, n);
    tc->readagain_len -= n;
}

struct asciicompat_encoding_t {
    const char *ascii_compat_name;
    const char *ascii_incompat_name;
};

static int
asciicompat_encoding_i(st_data_t key, st_data_t val, st_data_t arg)
{
    struct asciicompat_encoding_t *data = (struct asciicompat_encoding_t *)arg;
    transcoder_entry_t *entry = (transcoder_entry_t *)val;
    const rb_transcoder *tr;

    if (DECORATOR_P(entry->sname, entry->dname))
        return ST_CONTINUE;
    tr = load_transcoder_entry(entry);
    if (tr && tr->asciicompat_type == asciicompat_decoder) {
        data->ascii_compat_name = tr->dst_encoding;
        return ST_STOP;
    }
    return ST_CONTINUE;
}

const char *
rb_econv_asciicompat_encoding(const char *ascii_incompat_name)
{
    st_data_t v;
    st_table *table2;
    struct asciicompat_encoding_t data;

    if (!st_lookup(transcoder_table, (st_data_t)ascii_incompat_name, &v))
        return NULL;
    table2 = (st_table *)v;

    /*
     * Assumption:
     * There is at most one transcoder for
     * converting from ASCII incompatible encoding.
     *
     * For ISO-2022-JP, there is ISO-2022-JP -> stateless-ISO-2022-JP and no others.
     */
    if (table2->num_entries != 1)
        return NULL;

    data.ascii_incompat_name = ascii_incompat_name;
    data.ascii_compat_name = NULL;
    st_foreach(table2, asciicompat_encoding_i, (st_data_t)&data);
    return data.ascii_compat_name;
}

VALUE
rb_econv_substr_append(rb_econv_t *ec, VALUE src, long off, long len, VALUE dst, int flags)
{
    unsigned const char *ss, *sp, *se;
    unsigned char *ds, *dp, *de;
    rb_econv_result_t res;
    int max_output;

    if (NIL_P(dst)) {
        dst = rb_str_buf_new(len);
        if (ec->destination_encoding)
            rb_enc_associate(dst, ec->destination_encoding);
    }

    if (ec->last_tc)
        max_output = ec->last_tc->transcoder->max_output;
    else
        max_output = 1;

    res = econv_destination_buffer_full;
    while (res == econv_destination_buffer_full) {
        long dlen = RSTRING_LEN(dst);
        if (rb_str_capacity(dst) - dlen < (size_t)len + max_output) {
            unsigned long new_capa = (unsigned long)dlen + len + max_output;
            if (LONG_MAX < new_capa)
                rb_raise(rb_eArgError, "too long string");
            rb_str_resize(dst, new_capa);
            rb_str_set_len(dst, dlen);
        }
        ss = sp = (const unsigned char *)RSTRING_PTR(src) + off;
        se = ss + len;
        ds = (unsigned char *)RSTRING_PTR(dst);
        de = ds + rb_str_capacity(dst);
        dp = ds += dlen;
        res = rb_econv_convert(ec, &sp, se, &dp, de, flags);
        off += sp - ss;
        len -= sp - ss;
        rb_str_set_len(dst, dlen + (dp - ds));
        rb_econv_check_error(ec);
    }

    return dst;
}

VALUE
rb_econv_str_append(rb_econv_t *ec, VALUE src, VALUE dst, int flags)
{
    return rb_econv_substr_append(ec, src, 0, RSTRING_LEN(src), dst, flags);
}

VALUE
rb_econv_substr_convert(rb_econv_t *ec, VALUE src, long byteoff, long bytesize, int flags)
{
    return rb_econv_substr_append(ec, src, byteoff, bytesize, Qnil, flags);
}

VALUE
rb_econv_str_convert(rb_econv_t *ec, VALUE src, int flags)
{
    return rb_econv_substr_append(ec, src, 0, RSTRING_LEN(src), Qnil, flags);
}

static int
rb_econv_add_converter(rb_econv_t *ec, const char *sname, const char *dname, int n)
{
    transcoder_entry_t *entry;
    const rb_transcoder *tr;

    if (ec->started != 0)
        return -1;

    entry = get_transcoder_entry(sname, dname);
    if (!entry)
        return -1;

    tr = load_transcoder_entry(entry);
    if (!tr) return -1;

    return rb_econv_add_transcoder_at(ec, tr, n);
}

static int
rb_econv_decorate_at(rb_econv_t *ec, const char *decorator_name, int n)
{
    return rb_econv_add_converter(ec, "", decorator_name, n);
}

int
rb_econv_decorate_at_first(rb_econv_t *ec, const char *decorator_name)
{
    const rb_transcoder *tr;

    if (ec->num_trans == 0)
        return rb_econv_decorate_at(ec, decorator_name, 0);

    tr = ec->elems[0].tc->transcoder;

    if (!DECORATOR_P(tr->src_encoding, tr->dst_encoding) &&
        tr->asciicompat_type == asciicompat_decoder)
        return rb_econv_decorate_at(ec, decorator_name, 1);

    return rb_econv_decorate_at(ec, decorator_name, 0);
}

int
rb_econv_decorate_at_last(rb_econv_t *ec, const char *decorator_name)
{
    const rb_transcoder *tr;

    if (ec->num_trans == 0)
        return rb_econv_decorate_at(ec, decorator_name, 0);

    tr = ec->elems[ec->num_trans-1].tc->transcoder;

    if (!DECORATOR_P(tr->src_encoding, tr->dst_encoding) &&
        tr->asciicompat_type == asciicompat_encoder)
        return rb_econv_decorate_at(ec, decorator_name, ec->num_trans-1);

    return rb_econv_decorate_at(ec, decorator_name, ec->num_trans);
}

void
rb_econv_binmode(rb_econv_t *ec)
{
    const char *dname = 0;

    switch (ec->flags & ECONV_NEWLINE_DECORATOR_MASK) {
      case ECONV_UNIVERSAL_NEWLINE_DECORATOR:
	dname = "universal_newline";
	break;
      case ECONV_CRLF_NEWLINE_DECORATOR:
	dname = "crlf_newline";
	break;
      case ECONV_CR_NEWLINE_DECORATOR:
	dname = "cr_newline";
	break;
    }

    if (dname) {
        const rb_transcoder *transcoder = get_transcoder_entry("", dname)->transcoder;
        int num_trans = ec->num_trans;
	int i, j = 0;

	for (i=0; i < num_trans; i++) {
	    if (transcoder == ec->elems[i].tc->transcoder) {
		rb_transcoding_close(ec->elems[i].tc);
		xfree(ec->elems[i].out_buf_start);
		ec->num_trans--;
	    }
	    else
		ec->elems[j++] = ec->elems[i];
	}
    }

    ec->flags &= ~ECONV_NEWLINE_DECORATOR_MASK;
}

static VALUE
econv_description(const char *sname, const char *dname, int ecflags, VALUE mesg)
{
    int has_description = 0;

    if (NIL_P(mesg))
        mesg = rb_str_new(NULL, 0);

    if (*sname != '\0' || *dname != '\0') {
        if (*sname == '\0')
            rb_str_cat2(mesg, dname);
        else if (*dname == '\0')
            rb_str_cat2(mesg, sname);
        else
            rb_str_catf(mesg, "%s to %s", sname, dname);
        has_description = 1;
    }

    if (ecflags & (ECONV_NEWLINE_DECORATOR_MASK|
                   ECONV_XML_TEXT_DECORATOR|
                   ECONV_XML_ATTR_CONTENT_DECORATOR|
                   ECONV_XML_ATTR_QUOTE_DECORATOR)) {
        const char *pre = "";
        if (has_description)
            rb_str_cat2(mesg, " with ");
        if (ecflags & ECONV_UNIVERSAL_NEWLINE_DECORATOR)  {
            rb_str_cat2(mesg, pre); pre = ",";
            rb_str_cat2(mesg, "universal_newline");
        }
        if (ecflags & ECONV_CRLF_NEWLINE_DECORATOR) {
            rb_str_cat2(mesg, pre); pre = ",";
            rb_str_cat2(mesg, "crlf_newline");
        }
        if (ecflags & ECONV_CR_NEWLINE_DECORATOR) {
            rb_str_cat2(mesg, pre); pre = ",";
            rb_str_cat2(mesg, "cr_newline");
        }
        if (ecflags & ECONV_XML_TEXT_DECORATOR) {
            rb_str_cat2(mesg, pre); pre = ",";
            rb_str_cat2(mesg, "xml_text");
        }
        if (ecflags & ECONV_XML_ATTR_CONTENT_DECORATOR) {
            rb_str_cat2(mesg, pre); pre = ",";
            rb_str_cat2(mesg, "xml_attr_content");
        }
        if (ecflags & ECONV_XML_ATTR_QUOTE_DECORATOR) {
            rb_str_cat2(mesg, pre); pre = ",";
            rb_str_cat2(mesg, "xml_attr_quote");
        }
        has_description = 1;
    }
    if (!has_description) {
        rb_str_cat2(mesg, "no-conversion");
    }

    return mesg;
}

VALUE
rb_econv_open_exc(const char *sname, const char *dname, int ecflags)
{
    VALUE mesg, exc;
    mesg = rb_str_new_cstr("code converter not found (");
    econv_description(sname, dname, ecflags, mesg);
    rb_str_cat2(mesg, ")");
    exc = rb_exc_new3(rb_eConverterNotFoundError, mesg);
    return exc;
}

static VALUE
make_econv_exception(rb_econv_t *ec)
{
    VALUE mesg, exc;
    if (ec->last_error.result == econv_invalid_byte_sequence ||
        ec->last_error.result == econv_incomplete_input) {
        const char *err = (const char *)ec->last_error.error_bytes_start;
        size_t error_len = ec->last_error.error_bytes_len;
        VALUE bytes = rb_str_new(err, error_len);
        VALUE dumped = rb_str_dump(bytes);
        size_t readagain_len = ec->last_error.readagain_len;
        VALUE bytes2 = Qnil;
        VALUE dumped2;
        int idx;
        if (ec->last_error.result == econv_incomplete_input) {
            mesg = rb_sprintf("incomplete %s on %s",
                    StringValueCStr(dumped),
                    ec->last_error.source_encoding);
        }
        else if (readagain_len) {
            bytes2 = rb_str_new(err+error_len, readagain_len);
            dumped2 = rb_str_dump(bytes2);
            mesg = rb_sprintf("%s followed by %s on %s",
                    StringValueCStr(dumped),
                    StringValueCStr(dumped2),
                    ec->last_error.source_encoding);
        }
        else {
            mesg = rb_sprintf("%s on %s",
                    StringValueCStr(dumped),
                    ec->last_error.source_encoding);
        }

        exc = rb_exc_new3(rb_eInvalidByteSequenceError, mesg);
        rb_ivar_set(exc, rb_intern("error_bytes"), bytes);
        rb_ivar_set(exc, rb_intern("readagain_bytes"), bytes2);
        rb_ivar_set(exc, rb_intern("incomplete_input"), ec->last_error.result == econv_incomplete_input ? Qtrue : Qfalse);

      set_encs:
        rb_ivar_set(exc, rb_intern("source_encoding_name"), rb_str_new2(ec->last_error.source_encoding));
        rb_ivar_set(exc, rb_intern("destination_encoding_name"), rb_str_new2(ec->last_error.destination_encoding));
        idx = rb_enc_find_index(ec->last_error.source_encoding);
        if (0 <= idx)
            rb_ivar_set(exc, rb_intern("source_encoding"), rb_enc_from_encoding(rb_enc_from_index(idx)));
        idx = rb_enc_find_index(ec->last_error.destination_encoding);
        if (0 <= idx)
            rb_ivar_set(exc, rb_intern("destination_encoding"), rb_enc_from_encoding(rb_enc_from_index(idx)));
        return exc;
    }
    if (ec->last_error.result == econv_undefined_conversion) {
        VALUE bytes = rb_str_new((const char *)ec->last_error.error_bytes_start,
                                 ec->last_error.error_bytes_len);
        VALUE dumped = Qnil;
        int idx;
        if (strcmp(ec->last_error.source_encoding, "UTF-8") == 0) {
            rb_encoding *utf8 = rb_utf8_encoding();
            const char *start, *end;
            int n;
            start = (const char *)ec->last_error.error_bytes_start;
            end = start + ec->last_error.error_bytes_len;
            n = rb_enc_precise_mbclen(start, end, utf8);
            if (MBCLEN_CHARFOUND_P(n) &&
                (size_t)MBCLEN_CHARFOUND_LEN(n) == ec->last_error.error_bytes_len) {
                unsigned int cc = rb_enc_mbc_to_codepoint(start, end, utf8);
                dumped = rb_sprintf("U+%04X", cc);
            }
        }
        if (dumped == Qnil)
            dumped = rb_str_dump(bytes);
        if (strcmp(ec->last_error.source_encoding,
                   ec->source_encoding_name) == 0 &&
            strcmp(ec->last_error.destination_encoding,
                   ec->destination_encoding_name) == 0) {
            mesg = rb_sprintf("%s from %s to %s",
                    StringValueCStr(dumped),
                    ec->last_error.source_encoding,
                    ec->last_error.destination_encoding);
        }
        else {
            int i;
            mesg = rb_sprintf("%s to %s in conversion from %s",
                    StringValueCStr(dumped),
                    ec->last_error.destination_encoding,
                    ec->source_encoding_name);
            for (i = 0; i < ec->num_trans; i++) {
                const rb_transcoder *tr = ec->elems[i].tc->transcoder;
                if (!DECORATOR_P(tr->src_encoding, tr->dst_encoding))
                    rb_str_catf(mesg, " to %s",
                                ec->elems[i].tc->transcoder->dst_encoding);
            }
        }
        exc = rb_exc_new3(rb_eUndefinedConversionError, mesg);
        idx = rb_enc_find_index(ec->last_error.source_encoding);
        if (0 <= idx)
            rb_enc_associate_index(bytes, idx);
        rb_ivar_set(exc, rb_intern("error_char"), bytes);
        goto set_encs;
    }
    return Qnil;
}

static void
more_output_buffer(
        VALUE destination,
        unsigned char *(*resize_destination)(VALUE, size_t, size_t),
        int max_output,
        unsigned char **out_start_ptr,
        unsigned char **out_pos,
        unsigned char **out_stop_ptr)
{
    size_t len = (*out_pos - *out_start_ptr);
    size_t new_len = (len + max_output) * 2;
    *out_start_ptr = resize_destination(destination, len, new_len);
    *out_pos = *out_start_ptr + len;
    *out_stop_ptr = *out_start_ptr + new_len;
}

static int
make_replacement(rb_econv_t *ec)
{
    rb_transcoding *tc;
    const rb_transcoder *tr;
    const unsigned char *replacement;
    const char *repl_enc;
    const char *ins_enc;
    size_t len;

    if (ec->replacement_str)
        return 0;

    ins_enc = rb_econv_encoding_to_insert_output(ec);

    tc = ec->last_tc;
    if (*ins_enc) {
        tr = tc->transcoder;
        rb_enc_find(tr->dst_encoding);
        replacement = (const unsigned char *)get_replacement_character(ins_enc, &len, &repl_enc);
    }
    else {
        replacement = (unsigned char *)"?";
        len = 1;
        repl_enc = "";
    }

    ec->replacement_str = replacement;
    ec->replacement_len = len;
    ec->replacement_enc = repl_enc;
    ec->replacement_allocated = 0;
    return 0;
}

int
rb_econv_set_replacement(rb_econv_t *ec,
    const unsigned char *str, size_t len, const char *encname)
{
    unsigned char *str2;
    size_t len2;
    const char *encname2;

    encname2 = rb_econv_encoding_to_insert_output(ec);

    if (encoding_equal(encname, encname2)) {
        str2 = xmalloc(len);
        MEMCPY(str2, str, unsigned char, len); /* xxx: str may be invalid */
        len2 = len;
        encname2 = encname;
    }
    else {
        str2 = allocate_converted_string(encname, encname2, str, len, NULL, 0, &len2);
        if (!str2)
            return -1;
    }

    if (ec->replacement_allocated) {
        xfree((void *)ec->replacement_str);
    }
    ec->replacement_allocated = 1;
    ec->replacement_str = str2;
    ec->replacement_len = len2;
    ec->replacement_enc = encname2;
    return 0;
}

static int
output_replacement_character(rb_econv_t *ec)
{
    int ret;

    if (make_replacement(ec) == -1)
        return -1;

    ret = rb_econv_insert_output(ec, ec->replacement_str, ec->replacement_len, ec->replacement_enc);
    if (ret == -1)
        return -1;

    return 0;
}

#if 1
#define hash_fallback rb_hash_aref

static VALUE
proc_fallback(VALUE fallback, VALUE c)
{
    return rb_proc_call(fallback, rb_ary_new4(1, &c));
}

static VALUE
method_fallback(VALUE fallback, VALUE c)
{
    return rb_method_call(1, &c, fallback);
}

static VALUE
aref_fallback(VALUE fallback, VALUE c)
{
    return rb_funcall3(fallback, sym_aref, 1, &c);
}

static void
transcode_loop(const unsigned char **in_pos, unsigned char **out_pos,
	       const unsigned char *in_stop, unsigned char *out_stop,
               VALUE destination,
               unsigned char *(*resize_destination)(VALUE, size_t, size_t),
               const char *src_encoding,
               const char *dst_encoding,
               int ecflags,
               VALUE ecopts)
{
    rb_econv_t *ec;
    rb_transcoding *last_tc;
    rb_econv_result_t ret;
    unsigned char *out_start = *out_pos;
    int max_output;
    VALUE exc;
    VALUE fallback = Qnil;
    VALUE (*fallback_func)(VALUE, VALUE) = 0;

    ec = rb_econv_open_opts(src_encoding, dst_encoding, ecflags, ecopts);
    if (!ec)
        rb_exc_raise(rb_econv_open_exc(src_encoding, dst_encoding, ecflags));

    if (!NIL_P(ecopts) && RB_TYPE_P(ecopts, T_HASH)) {
	fallback = rb_hash_aref(ecopts, sym_fallback);
	if (RB_TYPE_P(fallback, T_HASH)) {
	    fallback_func = hash_fallback;
	}
	else if (rb_obj_is_proc(fallback)) {
	    fallback_func = proc_fallback;
	}
	else if (rb_obj_is_method(fallback)) {
	    fallback_func = method_fallback;
	}
	else {
	    fallback_func = aref_fallback;
	}
    }
    last_tc = ec->last_tc;
    max_output = last_tc ? last_tc->transcoder->max_output : 1;

  resume:
    ret = rb_econv_convert(ec, in_pos, in_stop, out_pos, out_stop, 0);

    if (!NIL_P(fallback) && ret == econv_undefined_conversion) {
	VALUE rep = rb_enc_str_new(
		(const char *)ec->last_error.error_bytes_start,
		ec->last_error.error_bytes_len,
		rb_enc_find(ec->last_error.source_encoding));
	rep = (*fallback_func)(fallback, rep);
	if (rep != Qundef && !NIL_P(rep)) {
	    StringValue(rep);
	    ret = rb_econv_insert_output(ec, (const unsigned char *)RSTRING_PTR(rep),
		    RSTRING_LEN(rep), rb_enc_name(rb_enc_get(rep)));
	    if ((int)ret == -1) {
		rb_raise(rb_eArgError, "too big fallback string");
	    }
	    goto resume;
	}
    }

    if (ret == econv_invalid_byte_sequence ||
        ret == econv_incomplete_input ||
        ret == econv_undefined_conversion) {
        exc = make_econv_exception(ec);
        rb_econv_close(ec);
	rb_exc_raise(exc);
    }

    if (ret == econv_destination_buffer_full) {
        more_output_buffer(destination, resize_destination, max_output, &out_start, out_pos, &out_stop);
        goto resume;
    }

    rb_econv_close(ec);
    return;
}
#else
/* sample transcode_loop implementation in byte-by-byte stream style */
static void
transcode_loop(const unsigned char **in_pos, unsigned char **out_pos,
	       const unsigned char *in_stop, unsigned char *out_stop,
               VALUE destination,
               unsigned char *(*resize_destination)(VALUE, size_t, size_t),
               const char *src_encoding,
               const char *dst_encoding,
               int ecflags,
               VALUE ecopts)
{
    rb_econv_t *ec;
    rb_transcoding *last_tc;
    rb_econv_result_t ret;
    unsigned char *out_start = *out_pos;
    const unsigned char *ptr;
    int max_output;
    VALUE exc;

    ec = rb_econv_open_opts(src_encoding, dst_encoding, ecflags, ecopts);
    if (!ec)
        rb_exc_raise(rb_econv_open_exc(src_encoding, dst_encoding, ecflags));

    last_tc = ec->last_tc;
    max_output = last_tc ? last_tc->transcoder->max_output : 1;

    ret = econv_source_buffer_empty;
    ptr = *in_pos;
    while (ret != econv_finished) {
        unsigned char input_byte;
        const unsigned char *p = &input_byte;

        if (ret == econv_source_buffer_empty) {
            if (ptr < in_stop) {
                input_byte = *ptr;
                ret = rb_econv_convert(ec, &p, p+1, out_pos, out_stop, ECONV_PARTIAL_INPUT);
            }
            else {
                ret = rb_econv_convert(ec, NULL, NULL, out_pos, out_stop, 0);
            }
        }
        else {
            ret = rb_econv_convert(ec, NULL, NULL, out_pos, out_stop, ECONV_PARTIAL_INPUT);
        }
        if (&input_byte != p)
            ptr += p - &input_byte;
        switch (ret) {
          case econv_invalid_byte_sequence:
          case econv_incomplete_input:
          case econv_undefined_conversion:
            exc = make_econv_exception(ec);
            rb_econv_close(ec);
            rb_exc_raise(exc);
            break;

          case econv_destination_buffer_full:
            more_output_buffer(destination, resize_destination, max_output, &out_start, out_pos, &out_stop);
            break;

          case econv_source_buffer_empty:
            break;

          case econv_finished:
            break;
        }
    }
    rb_econv_close(ec);
    *in_pos = in_stop;
    return;
}
#endif


/*
 *  String-specific code
 */

static unsigned char *
str_transcoding_resize(VALUE destination, size_t len, size_t new_len)
{
    rb_str_resize(destination, new_len);
    return (unsigned char *)RSTRING_PTR(destination);
}

static int
econv_opts(VALUE opt, int ecflags)
{
    VALUE v;

    v = rb_hash_aref(opt, sym_invalid);
    if (NIL_P(v)) {
    }
    else if (v==sym_replace) {
        ecflags |= ECONV_INVALID_REPLACE;
    }
    else {
        rb_raise(rb_eArgError, "unknown value for invalid character option");
    }

    v = rb_hash_aref(opt, sym_undef);
    if (NIL_P(v)) {
    }
    else if (v==sym_replace) {
        ecflags |= ECONV_UNDEF_REPLACE;
    }
    else {
        rb_raise(rb_eArgError, "unknown value for undefined character option");
    }

    v = rb_hash_aref(opt, sym_replace);
    if (!NIL_P(v) && !(ecflags & ECONV_INVALID_REPLACE)) {
        ecflags |= ECONV_UNDEF_REPLACE;
    }

    v = rb_hash_aref(opt, sym_xml);
    if (!NIL_P(v)) {
        if (v==sym_text) {
            ecflags |= ECONV_XML_TEXT_DECORATOR|ECONV_UNDEF_HEX_CHARREF;
        }
        else if (v==sym_attr) {
            ecflags |= ECONV_XML_ATTR_CONTENT_DECORATOR|ECONV_XML_ATTR_QUOTE_DECORATOR|ECONV_UNDEF_HEX_CHARREF;
        }
        else if (RB_TYPE_P(v, T_SYMBOL)) {
            rb_raise(rb_eArgError, "unexpected value for xml option: %s", rb_id2name(SYM2ID(v)));
        }
        else {
            rb_raise(rb_eArgError, "unexpected value for xml option");
        }
    }

#ifdef ENABLE_ECONV_NEWLINE_OPTION
    v = rb_hash_aref(opt, sym_newline);
    if (!NIL_P(v)) {
	ecflags &= ~ECONV_NEWLINE_DECORATOR_MASK;
	if (v == sym_universal) {
	    ecflags |= ECONV_UNIVERSAL_NEWLINE_DECORATOR;
	}
	else if (v == sym_crlf) {
	    ecflags |= ECONV_CRLF_NEWLINE_DECORATOR;
	}
	else if (v == sym_cr) {
	    ecflags |= ECONV_CR_NEWLINE_DECORATOR;
	}
	else if (v == sym_lf) {
	    /* ecflags |= ECONV_LF_NEWLINE_DECORATOR; */
	}
	else if (SYMBOL_P(v)) {
	    rb_raise(rb_eArgError, "unexpected value for newline option: %s",
		     rb_id2name(SYM2ID(v)));
	}
	else {
	    rb_raise(rb_eArgError, "unexpected value for newline option");
	}
    }
    else
#endif
    {
	int setflags = 0, newlineflag = 0;

	v = rb_hash_aref(opt, sym_universal_newline);
	if (RTEST(v))
	    setflags |= ECONV_UNIVERSAL_NEWLINE_DECORATOR;
	newlineflag |= !NIL_P(v);

	v = rb_hash_aref(opt, sym_crlf_newline);
	if (RTEST(v))
	    setflags |= ECONV_CRLF_NEWLINE_DECORATOR;
	newlineflag |= !NIL_P(v);

	v = rb_hash_aref(opt, sym_cr_newline);
	if (RTEST(v))
	    setflags |= ECONV_CR_NEWLINE_DECORATOR;
	newlineflag |= !NIL_P(v);

	if (newlineflag) {
	    ecflags &= ~ECONV_NEWLINE_DECORATOR_MASK;
	    ecflags |= setflags;
	}
    }

    return ecflags;
}

int
rb_econv_prepare_options(VALUE opthash, VALUE *opts, int ecflags)
{
    VALUE newhash = Qnil;
    VALUE v;

    if (NIL_P(opthash)) {
        *opts = Qnil;
        return ecflags;
    }
    ecflags = econv_opts(opthash, ecflags);

    v = rb_hash_aref(opthash, sym_replace);
    if (!NIL_P(v)) {
	StringValue(v);
	if (rb_enc_str_coderange(v) == ENC_CODERANGE_BROKEN) {
	    VALUE dumped = rb_str_dump(v);
	    rb_raise(rb_eArgError, "replacement string is broken: %s as %s",
		     StringValueCStr(dumped),
		     rb_enc_name(rb_enc_get(v)));
	}
	v = rb_str_new_frozen(v);
	newhash = rb_hash_new();
	rb_hash_aset(newhash, sym_replace, v);
    }

    v = rb_hash_aref(opthash, sym_fallback);
    if (!NIL_P(v)) {
	VALUE h = rb_check_hash_type(v);
	if (NIL_P(h)
	    ? (rb_obj_is_proc(v) || rb_obj_is_method(v) || rb_respond_to(v, sym_aref))
	    : (v = h, 1)) {
	    if (NIL_P(newhash))
		newhash = rb_hash_new();
	    rb_hash_aset(newhash, sym_fallback, v);
	}
    }

    if (!NIL_P(newhash))
        rb_hash_freeze(newhash);
    *opts = newhash;

    return ecflags;
}

int
rb_econv_prepare_opts(VALUE opthash, VALUE *opts)
{
    return rb_econv_prepare_options(opthash, opts, 0);
}

rb_econv_t *
rb_econv_open_opts(const char *source_encoding, const char *destination_encoding, int ecflags, VALUE opthash)
{
    rb_econv_t *ec;
    VALUE replacement;

    if (NIL_P(opthash)) {
        replacement = Qnil;
    }
    else {
        if (!RB_TYPE_P(opthash, T_HASH) || !OBJ_FROZEN(opthash))
            rb_bug("rb_econv_open_opts called with invalid opthash");
        replacement = rb_hash_aref(opthash, sym_replace);
    }

    ec = rb_econv_open(source_encoding, destination_encoding, ecflags);
    if (!ec)
        return ec;

    if (!NIL_P(replacement)) {
        int ret;
        rb_encoding *enc = rb_enc_get(replacement);

        ret = rb_econv_set_replacement(ec,
                (const unsigned char *)RSTRING_PTR(replacement),
                RSTRING_LEN(replacement),
                rb_enc_name(enc));
        if (ret == -1) {
            rb_econv_close(ec);
            return NULL;
        }
    }
    return ec;
}

static int
enc_arg(volatile VALUE *arg, const char **name_p, rb_encoding **enc_p)
{
    rb_encoding *enc;
    const char *n;
    int encidx;
    VALUE encval;

    if (((encidx = rb_to_encoding_index(encval = *arg)) < 0) ||
	!(enc = rb_enc_from_index(encidx))) {
	enc = NULL;
	encidx = 0;
	n = StringValueCStr(*arg);
    }
    else {
	n = rb_enc_name(enc);
    }

    *name_p = n;
    *enc_p = enc;

    return encidx;
}

static int
str_transcode_enc_args(VALUE str, volatile VALUE *arg1, volatile VALUE *arg2,
        const char **sname_p, rb_encoding **senc_p,
        const char **dname_p, rb_encoding **denc_p)
{
    rb_encoding *senc, *denc;
    const char *sname, *dname;
    int sencidx, dencidx;

    dencidx = enc_arg(arg1, &dname, &denc);

    if (NIL_P(*arg2)) {
	sencidx = rb_enc_get_index(str);
	senc = rb_enc_from_index(sencidx);
	sname = rb_enc_name(senc);
    }
    else {
        sencidx = enc_arg(arg2, &sname, &senc);
    }

    *sname_p = sname;
    *senc_p = senc;
    *dname_p = dname;
    *denc_p = denc;
    return dencidx;
}

static int
str_transcode0(int argc, VALUE *argv, VALUE *self, int ecflags, VALUE ecopts)
{
    VALUE dest;
    VALUE str = *self;
    volatile VALUE arg1, arg2;
    long blen, slen;
    unsigned char *buf, *bp, *sp;
    const unsigned char *fromp;
    rb_encoding *senc, *denc;
    const char *sname, *dname;
    int dencidx;

    rb_check_arity(argc, 0, 2);

    if (argc == 0) {
	arg1 = rb_enc_default_internal();
	if (NIL_P(arg1)) {
	    if (!ecflags) return -1;
	    arg1 = rb_obj_encoding(str);
	}
	ecflags |= ECONV_INVALID_REPLACE | ECONV_UNDEF_REPLACE;
    }
    else {
	arg1 = argv[0];
    }
    arg2 = argc<=1 ? Qnil : argv[1];
    dencidx = str_transcode_enc_args(str, &arg1, &arg2, &sname, &senc, &dname, &denc);

    if ((ecflags & (ECONV_NEWLINE_DECORATOR_MASK|
                    ECONV_XML_TEXT_DECORATOR|
                    ECONV_XML_ATTR_CONTENT_DECORATOR|
                    ECONV_XML_ATTR_QUOTE_DECORATOR)) == 0) {
        if (senc && senc == denc) {
            return NIL_P(arg2) ? -1 : dencidx;
        }
        if (senc && denc && rb_enc_asciicompat(senc) && rb_enc_asciicompat(denc)) {
            if (rb_enc_str_coderange(str) == ENC_CODERANGE_7BIT) {
                return dencidx;
            }
        }
        if (encoding_equal(sname, dname)) {
            return NIL_P(arg2) ? -1 : dencidx;
        }
    }
    else {
        if (encoding_equal(sname, dname)) {
            sname = "";
            dname = "";
        }
    }

    fromp = sp = (unsigned char *)RSTRING_PTR(str);
    slen = RSTRING_LEN(str);
    blen = slen + 30; /* len + margin */
    dest = rb_str_tmp_new(blen);
    bp = (unsigned char *)RSTRING_PTR(dest);

    transcode_loop(&fromp, &bp, (sp+slen), (bp+blen), dest, str_transcoding_resize, sname, dname, ecflags, ecopts);
    if (fromp != sp+slen) {
        rb_raise(rb_eArgError, "not fully converted, %"PRIdPTRDIFF" bytes left", sp+slen-fromp);
    }
    buf = (unsigned char *)RSTRING_PTR(dest);
    *bp = '\0';
    rb_str_set_len(dest, bp - buf);

    /* set encoding */
    if (!denc) {
	dencidx = rb_define_dummy_encoding(dname);
    }
    *self = dest;

    return dencidx;
}

static int
str_transcode(int argc, VALUE *argv, VALUE *self)
{
    VALUE opt;
    int ecflags = 0;
    VALUE ecopts = Qnil;

    argc = rb_scan_args(argc, argv, "02:", NULL, NULL, &opt);
    if (!NIL_P(opt)) {
	ecflags = rb_econv_prepare_opts(opt, &ecopts);
    }
    return str_transcode0(argc, argv, self, ecflags, ecopts);
}

static inline VALUE
str_encode_associate(VALUE str, int encidx)
{
    int cr = 0;

    rb_enc_associate_index(str, encidx);

    /* transcoded string never be broken. */
    if (rb_enc_asciicompat(rb_enc_from_index(encidx))) {
	rb_str_coderange_scan_restartable(RSTRING_PTR(str), RSTRING_END(str), 0, &cr);
    }
    else {
	cr = ENC_CODERANGE_VALID;
    }
    ENC_CODERANGE_SET(str, cr);
    return str;
}

/*
 *  call-seq:
 *     str.encode!(encoding [, options] )   -> str
 *     str.encode!(dst_encoding, src_encoding [, options] )   -> str
 *
 *  The first form transcodes the contents of <i>str</i> from
 *  str.encoding to +encoding+.
 *  The second form transcodes the contents of <i>str</i> from
 *  src_encoding to dst_encoding.
 *  The options Hash gives details for conversion. See String#encode
 *  for details.
 *  Returns the string even if no changes were made.
 */

static VALUE
str_encode_bang(int argc, VALUE *argv, VALUE str)
{
    VALUE newstr;
    int encidx;

    rb_check_frozen(str);

    newstr = str;
    encidx = str_transcode(argc, argv, &newstr);

    if (encidx < 0) return str;
    rb_str_shared_replace(str, newstr);
    return str_encode_associate(str, encidx);
}

static VALUE encoded_dup(VALUE newstr, VALUE str, int encidx);

/*
 *  call-seq:
 *     str.encode(encoding [, options] )   -> str
 *     str.encode(dst_encoding, src_encoding [, options] )   -> str
 *     str.encode([options])   -> str
 *
 *  The first form returns a copy of +str+ transcoded
 *  to encoding +encoding+.
 *  The second form returns a copy of +str+ transcoded
 *  from src_encoding to dst_encoding.
 *  The last form returns a copy of +str+ transcoded to
 *  <tt>Encoding.default_internal</tt>.
 *
 *  By default, the first and second form raise
 *  Encoding::UndefinedConversionError for characters that are
 *  undefined in the destination encoding, and
 *  Encoding::InvalidByteSequenceError for invalid byte sequences
 *  in the source encoding. The last form by default does not raise
 *  exceptions but uses replacement strings.
 *
 *  The +options+ Hash gives details for conversion and can have the following
 *  keys:
 *
 *  :invalid ::
 *    If the value is +:replace+, #encode replaces invalid byte sequences in
 *    +str+ with the replacement character.  The default is to raise the
 *    Encoding::InvalidByteSequenceError exception
 *  :undef ::
 *    If the value is +:replace+, #encode replaces characters which are
 *    undefined in the destination encoding with the replacement character.
 *    The default is to raise the Encoding::UndefinedConversionError.
 *  :replace ::
 *    Sets the replacement string to the given value. The default replacement
 *    string is "\uFFFD" for Unicode encoding forms, and "?" otherwise.
 *  :fallback ::
 *    Sets the replacement string by the given object for undefined
 *    character.  The object should be a Hash, a Proc, a Method, or an
 *    object which has [] method.
 *    Its key is an undefined character encoded in the source encoding
 *    of current transcoder. Its value can be any encoding until it
 *    can be converted into the destination encoding of the transcoder.
 *  :xml ::
 *    The value must be +:text+ or +:attr+.
 *    If the value is +:text+ #encode replaces undefined characters with their
 *    (upper-case hexadecimal) numeric character references. '&', '<', and '>'
 *    are converted to "&amp;", "&lt;", and "&gt;", respectively.
 *    If the value is +:attr+, #encode also quotes the replacement result
 *    (using '"'), and replaces '"' with "&quot;".
 *  :cr_newline ::
 *    Replaces LF ("\n") with CR ("\r") if value is true.
 *  :crlf_newline ::
 *    Replaces LF ("\n") with CRLF ("\r\n") if value is true.
 *  :universal_newline ::
 *    Replaces CRLF ("\r\n") and CR ("\r") with LF ("\n") if value is true.
 */

static VALUE
str_encode(int argc, VALUE *argv, VALUE str)
{
    VALUE newstr = str;
    int encidx = str_transcode(argc, argv, &newstr);
    return encoded_dup(newstr, str, encidx);
}

VALUE
rb_str_encode(VALUE str, VALUE to, int ecflags, VALUE ecopts)
{
    int argc = 1;
    VALUE *argv = &to;
    VALUE newstr = str;
    int encidx = str_transcode0(argc, argv, &newstr, ecflags, ecopts);
    return encoded_dup(newstr, str, encidx);
}

static VALUE
encoded_dup(VALUE newstr, VALUE str, int encidx)
{
    if (encidx < 0) return rb_str_dup(str);
    if (newstr == str) {
	newstr = rb_str_dup(str);
    }
    else {
	RBASIC(newstr)->klass = rb_obj_class(str);
    }
    return str_encode_associate(newstr, encidx);
}

static void
econv_free(void *ptr)
{
    rb_econv_t *ec = ptr;
    rb_econv_close(ec);
}

static size_t
econv_memsize(const void *ptr)
{
    return ptr ? sizeof(rb_econv_t) : 0;
}

static const rb_data_type_t econv_data_type = {
    "econv",
    {NULL, econv_free, econv_memsize,},
};

static VALUE
econv_s_allocate(VALUE klass)
{
    return TypedData_Wrap_Struct(klass, &econv_data_type, NULL);
}

static rb_encoding *
make_dummy_encoding(const char *name)
{
    rb_encoding *enc;
    int idx;
    idx = rb_define_dummy_encoding(name);
    enc = rb_enc_from_index(idx);
    return enc;
}

static rb_encoding *
make_encoding(const char *name)
{
    rb_encoding *enc;
    enc = rb_enc_find(name);
    if (!enc)
        enc = make_dummy_encoding(name);
    return enc;
}

static VALUE
make_encobj(const char *name)
{
    return rb_enc_from_encoding(make_encoding(name));
}

/*
 * call-seq:
 *   Encoding::Converter.asciicompat_encoding(string) -> encoding or nil
 *   Encoding::Converter.asciicompat_encoding(encoding) -> encoding or nil
 *
 * Returns the corresponding ASCII compatible encoding.
 *
 * Returns nil if the argument is an ASCII compatible encoding.
 *
 * "corresponding ASCII compatible encoding" is an ASCII compatible encoding which
 * can represents exactly the same characters as the given ASCII incompatible encoding.
 * So, no conversion undefined error occurs when converting between the two encodings.
 *
 *   Encoding::Converter.asciicompat_encoding("ISO-2022-JP") #=> #<Encoding:stateless-ISO-2022-JP>
 *   Encoding::Converter.asciicompat_encoding("UTF-16BE") #=> #<Encoding:UTF-8>
 *   Encoding::Converter.asciicompat_encoding("UTF-8") #=> nil
 *
 */
static VALUE
econv_s_asciicompat_encoding(VALUE klass, VALUE arg)
{
    const char *arg_name, *result_name;
    rb_encoding *arg_enc, *result_enc;

    enc_arg(&arg, &arg_name, &arg_enc);

    result_name = rb_econv_asciicompat_encoding(arg_name);

    if (result_name == NULL)
        return Qnil;

    result_enc = make_encoding(result_name);

    return rb_enc_from_encoding(result_enc);
}

static void
econv_args(int argc, VALUE *argv,
    volatile VALUE *snamev_p, volatile VALUE *dnamev_p,
    const char **sname_p, const char **dname_p,
    rb_encoding **senc_p, rb_encoding **denc_p,
    int *ecflags_p,
    VALUE *ecopts_p)
{
    VALUE opt, flags_v, ecopts;
    int sidx, didx;
    const char *sname, *dname;
    rb_encoding *senc, *denc;
    int ecflags;

    argc = rb_scan_args(argc, argv, "21:", snamev_p, dnamev_p, &flags_v, &opt);

    if (!NIL_P(flags_v)) {
	if (!NIL_P(opt)) {
	    rb_error_arity(argc + 1, 2, 3);
	}
        ecflags = NUM2INT(rb_to_int(flags_v));
        ecopts = Qnil;
    }
    else if (!NIL_P(opt)) {
        ecflags = rb_econv_prepare_opts(opt, &ecopts);
    }
    else {
        ecflags = 0;
        ecopts = Qnil;
    }

    senc = NULL;
    sidx = rb_to_encoding_index(*snamev_p);
    if (0 <= sidx) {
        senc = rb_enc_from_index(sidx);
    }
    else {
        StringValue(*snamev_p);
    }

    denc = NULL;
    didx = rb_to_encoding_index(*dnamev_p);
    if (0 <= didx) {
        denc = rb_enc_from_index(didx);
    }
    else {
        StringValue(*dnamev_p);
    }

    sname = senc ? rb_enc_name(senc) : StringValueCStr(*snamev_p);
    dname = denc ? rb_enc_name(denc) : StringValueCStr(*dnamev_p);

    *sname_p = sname;
    *dname_p = dname;
    *senc_p = senc;
    *denc_p = denc;
    *ecflags_p = ecflags;
    *ecopts_p = ecopts;
}

static int
decorate_convpath(VALUE convpath, int ecflags)
{
    int num_decorators;
    const char *decorators[MAX_ECFLAGS_DECORATORS];
    int i;
    int n, len;

    num_decorators = decorator_names(ecflags, decorators);
    if (num_decorators == -1)
        return -1;

    len = n = RARRAY_LENINT(convpath);
    if (n != 0) {
        VALUE pair = RARRAY_PTR(convpath)[n-1];
	if (RB_TYPE_P(pair, T_ARRAY)) {
	    const char *sname = rb_enc_name(rb_to_encoding(RARRAY_PTR(pair)[0]));
	    const char *dname = rb_enc_name(rb_to_encoding(RARRAY_PTR(pair)[1]));
	    transcoder_entry_t *entry = get_transcoder_entry(sname, dname);
	    const rb_transcoder *tr = load_transcoder_entry(entry);
	    if (!tr)
		return -1;
	    if (!DECORATOR_P(tr->src_encoding, tr->dst_encoding) &&
		    tr->asciicompat_type == asciicompat_encoder) {
		n--;
		rb_ary_store(convpath, len + num_decorators - 1, pair);
	    }
	}
	else {
	    rb_ary_store(convpath, len + num_decorators - 1, pair);
	}
    }

    for (i = 0; i < num_decorators; i++)
        rb_ary_store(convpath, n + i, rb_str_new_cstr(decorators[i]));

    return 0;
}

static void
search_convpath_i(const char *sname, const char *dname, int depth, void *arg)
{
    VALUE *ary_p = arg;
    VALUE v;

    if (*ary_p == Qnil) {
        *ary_p = rb_ary_new();
    }

    if (DECORATOR_P(sname, dname)) {
        v = rb_str_new_cstr(dname);
    }
    else {
        v = rb_assoc_new(make_encobj(sname), make_encobj(dname));
    }
    rb_ary_store(*ary_p, depth, v);
}

/*
 * call-seq:
 *   Encoding::Converter.search_convpath(source_encoding, destination_encoding)         -> ary
 *   Encoding::Converter.search_convpath(source_encoding, destination_encoding, opt)    -> ary
 *
 *  Returns a conversion path.
 *
 *   p Encoding::Converter.search_convpath("ISO-8859-1", "EUC-JP")
 *   #=> [[#<Encoding:ISO-8859-1>, #<Encoding:UTF-8>],
 *   #    [#<Encoding:UTF-8>, #<Encoding:EUC-JP>]]
 *
 *   p Encoding::Converter.search_convpath("ISO-8859-1", "EUC-JP", universal_newline: true)
 *   or
 *   p Encoding::Converter.search_convpath("ISO-8859-1", "EUC-JP", newline: :universal)
 *   #=> [[#<Encoding:ISO-8859-1>, #<Encoding:UTF-8>],
 *   #    [#<Encoding:UTF-8>, #<Encoding:EUC-JP>],
 *   #    "universal_newline"]
 *
 *   p Encoding::Converter.search_convpath("ISO-8859-1", "UTF-32BE", universal_newline: true)
 *   or
 *   p Encoding::Converter.search_convpath("ISO-8859-1", "UTF-32BE", newline: :universal)
 *   #=> [[#<Encoding:ISO-8859-1>, #<Encoding:UTF-8>],
 *   #    "universal_newline",
 *   #    [#<Encoding:UTF-8>, #<Encoding:UTF-32BE>]]
 */
static VALUE
econv_s_search_convpath(int argc, VALUE *argv, VALUE klass)
{
    volatile VALUE snamev, dnamev;
    const char *sname, *dname;
    rb_encoding *senc, *denc;
    int ecflags;
    VALUE ecopts;
    VALUE convpath;

    econv_args(argc, argv, &snamev, &dnamev, &sname, &dname, &senc, &denc, &ecflags, &ecopts);

    convpath = Qnil;
    transcode_search_path(sname, dname, search_convpath_i, &convpath);

    if (NIL_P(convpath))
        rb_exc_raise(rb_econv_open_exc(sname, dname, ecflags));

    if (decorate_convpath(convpath, ecflags) == -1)
        rb_exc_raise(rb_econv_open_exc(sname, dname, ecflags));

    return convpath;
}

/*
 * Check the existence of a conversion path.
 * Returns the number of converters in the conversion path.
 * result: >=0:success -1:failure
 */
int
rb_econv_has_convpath_p(const char* from_encoding, const char* to_encoding)
{
    VALUE convpath = Qnil;
    transcode_search_path(from_encoding, to_encoding, search_convpath_i,
			  &convpath);
    return RTEST(convpath);
}

struct rb_econv_init_by_convpath_t {
    rb_econv_t *ec;
    int index;
    int ret;
};

static void
rb_econv_init_by_convpath_i(const char *sname, const char *dname, int depth, void *arg)
{
    struct rb_econv_init_by_convpath_t *a = (struct rb_econv_init_by_convpath_t *)arg;
    int ret;

    if (a->ret == -1)
        return;

    ret = rb_econv_add_converter(a->ec, sname, dname, a->index);

    a->ret = ret;
    return;
}

static rb_econv_t *
rb_econv_init_by_convpath(VALUE self, VALUE convpath,
    const char **sname_p, const char **dname_p,
    rb_encoding **senc_p, rb_encoding**denc_p)
{
    rb_econv_t *ec;
    long i;
    int ret, first=1;
    VALUE elt;
    rb_encoding *senc = 0, *denc = 0;
    const char *sname, *dname;

    ec = rb_econv_alloc(RARRAY_LENINT(convpath));
    DATA_PTR(self) = ec;

    for (i = 0; i < RARRAY_LEN(convpath); i++) {
        volatile VALUE snamev, dnamev;
        VALUE pair;
        elt = rb_ary_entry(convpath, i);
        if (!NIL_P(pair = rb_check_array_type(elt))) {
            if (RARRAY_LEN(pair) != 2)
                rb_raise(rb_eArgError, "not a 2-element array in convpath");
            snamev = rb_ary_entry(pair, 0);
            enc_arg(&snamev, &sname, &senc);
            dnamev = rb_ary_entry(pair, 1);
            enc_arg(&dnamev, &dname, &denc);
        }
        else {
            sname = "";
            dname = StringValueCStr(elt);
        }
        if (DECORATOR_P(sname, dname)) {
            ret = rb_econv_add_converter(ec, sname, dname, ec->num_trans);
            if (ret == -1)
                rb_raise(rb_eArgError, "decoration failed: %s", dname);
        }
        else {
            int j = ec->num_trans;
            struct rb_econv_init_by_convpath_t arg;
            arg.ec = ec;
            arg.index = ec->num_trans;
            arg.ret = 0;
            ret = transcode_search_path(sname, dname, rb_econv_init_by_convpath_i, &arg);
            if (ret == -1 || arg.ret == -1)
                rb_raise(rb_eArgError, "adding conversion failed: %s to %s", sname, dname);
            if (first) {
                first = 0;
                *senc_p = senc;
                *sname_p = ec->elems[j].tc->transcoder->src_encoding;
            }
            *denc_p = denc;
            *dname_p = ec->elems[ec->num_trans-1].tc->transcoder->dst_encoding;
        }
    }

    if (first) {
      *senc_p = NULL;
      *denc_p = NULL;
      *sname_p = "";
      *dname_p = "";
    }

    ec->source_encoding_name = *sname_p;
    ec->destination_encoding_name = *dname_p;

    return ec;
}

/*
 * call-seq:
 *   Encoding::Converter.new(source_encoding, destination_encoding)
 *   Encoding::Converter.new(source_encoding, destination_encoding, opt)
 *   Encoding::Converter.new(convpath)
 *
 * possible options elements:
 *   hash form:
 *     :invalid => nil            # raise error on invalid byte sequence (default)
 *     :invalid => :replace       # replace invalid byte sequence
 *     :undef => nil              # raise error on undefined conversion (default)
 *     :undef => :replace         # replace undefined conversion
 *     :replace => string         # replacement string ("?" or "\uFFFD" if not specified)
 *     :newline => :universal     # decorator for converting CRLF and CR to LF
 *     :newline => :crlf          # decorator for converting LF to CRLF
 *     :newline => :cr            # decorator for converting LF to CR
 *     :universal_newline => true # decorator for converting CRLF and CR to LF
 *     :crlf_newline => true      # decorator for converting LF to CRLF
 *     :cr_newline => true        # decorator for converting LF to CR
 *     :xml => :text              # escape as XML CharData.
 *     :xml => :attr              # escape as XML AttValue
 *   integer form:
 *     Encoding::Converter::INVALID_REPLACE
 *     Encoding::Converter::UNDEF_REPLACE
 *     Encoding::Converter::UNDEF_HEX_CHARREF
 *     Encoding::Converter::UNIVERSAL_NEWLINE_DECORATOR
 *     Encoding::Converter::CRLF_NEWLINE_DECORATOR
 *     Encoding::Converter::CR_NEWLINE_DECORATOR
 *     Encoding::Converter::XML_TEXT_DECORATOR
 *     Encoding::Converter::XML_ATTR_CONTENT_DECORATOR
 *     Encoding::Converter::XML_ATTR_QUOTE_DECORATOR
 *
 * Encoding::Converter.new creates an instance of Encoding::Converter.
 *
 * Source_encoding and destination_encoding should be a string or
 * Encoding object.
 *
 * opt should be nil, a hash or an integer.
 *
 * convpath should be an array.
 * convpath may contain
 * - two-element arrays which contain encodings or encoding names, or
 * - strings representing decorator names.
 *
 * Encoding::Converter.new optionally takes an option.
 * The option should be a hash or an integer.
 * The option hash can contain :invalid => nil, etc.
 * The option integer should be logical-or of constants such as
 * Encoding::Converter::INVALID_REPLACE, etc.
 *
 * [:invalid => nil]
 *   Raise error on invalid byte sequence.  This is a default behavior.
 * [:invalid => :replace]
 *   Replace invalid byte sequence by replacement string.
 * [:undef => nil]
 *   Raise an error if a character in source_encoding is not defined in destination_encoding.
 *   This is a default behavior.
 * [:undef => :replace]
 *   Replace undefined character in destination_encoding with replacement string.
 * [:replace => string]
 *   Specify the replacement string.
 *   If not specified, "\uFFFD" is used for Unicode encodings and "?" for others.
 * [:universal_newline => true]
 *   Convert CRLF and CR to LF.
 * [:crlf_newline => true]
 *   Convert LF to CRLF.
 * [:cr_newline => true]
 *   Convert LF to CR.
 * [:xml => :text]
 *   Escape as XML CharData.
 *   This form can be used as a HTML 4.0 #PCDATA.
 *   - '&' -> '&amp;'
 *   - '<' -> '&lt;'
 *   - '>' -> '&gt;'
 *   - undefined characters in destination_encoding -> hexadecimal CharRef such as &#xHH;
 * [:xml => :attr]
 *   Escape as XML AttValue.
 *   The converted result is quoted as "...".
 *   This form can be used as a HTML 4.0 attribute value.
 *   - '&' -> '&amp;'
 *   - '<' -> '&lt;'
 *   - '>' -> '&gt;'
 *   - '"' -> '&quot;'
 *   - undefined characters in destination_encoding -> hexadecimal CharRef such as &#xHH;
 *
 * Examples:
 *   # UTF-16BE to UTF-8
 *   ec = Encoding::Converter.new("UTF-16BE", "UTF-8")
 *
 *   # Usually, decorators such as newline conversion are inserted last.
 *   ec = Encoding::Converter.new("UTF-16BE", "UTF-8", :universal_newline => true)
 *   p ec.convpath #=> [[#<Encoding:UTF-16BE>, #<Encoding:UTF-8>],
 *                 #    "universal_newline"]
 *
 *   # But, if the last encoding is ASCII incompatible,
 *   # decorators are inserted before the last conversion.
 *   ec = Encoding::Converter.new("UTF-8", "UTF-16BE", :crlf_newline => true)
 *   p ec.convpath #=> ["crlf_newline",
 *                 #    [#<Encoding:UTF-8>, #<Encoding:UTF-16BE>]]
 *
 *   # Conversion path can be specified directly.
 *   ec = Encoding::Converter.new(["universal_newline", ["EUC-JP", "UTF-8"], ["UTF-8", "UTF-16BE"]])
 *   p ec.convpath #=> ["universal_newline",
 *                 #    [#<Encoding:EUC-JP>, #<Encoding:UTF-8>],
 *                 #    [#<Encoding:UTF-8>, #<Encoding:UTF-16BE>]]
 */
static VALUE
econv_init(int argc, VALUE *argv, VALUE self)
{
    VALUE ecopts;
    volatile VALUE snamev, dnamev;
    const char *sname, *dname;
    rb_encoding *senc, *denc;
    rb_econv_t *ec;
    int ecflags;
    VALUE convpath;

    if (rb_check_typeddata(self, &econv_data_type)) {
        rb_raise(rb_eTypeError, "already initialized");
    }

    if (argc == 1 && !NIL_P(convpath = rb_check_array_type(argv[0]))) {
        ec = rb_econv_init_by_convpath(self, convpath, &sname, &dname, &senc, &denc);
        ecflags = 0;
        ecopts = Qnil;
    }
    else {
        econv_args(argc, argv, &snamev, &dnamev, &sname, &dname, &senc, &denc, &ecflags, &ecopts);
        ec = rb_econv_open_opts(sname, dname, ecflags, ecopts);
    }

    if (!ec) {
        rb_exc_raise(rb_econv_open_exc(sname, dname, ecflags));
    }

    if (!DECORATOR_P(sname, dname)) {
        if (!senc)
            senc = make_dummy_encoding(sname);
        if (!denc)
            denc = make_dummy_encoding(dname);
    }

    ec->source_encoding = senc;
    ec->destination_encoding = denc;

    DATA_PTR(self) = ec;

    return self;
}

/*
 * call-seq:
 *   ec.inspect         -> string
 *
 * Returns a printable version of <i>ec</i>
 *
 *   ec = Encoding::Converter.new("iso-8859-1", "utf-8")
 *   puts ec.inspect    #=> #<Encoding::Converter: ISO-8859-1 to UTF-8>
 *
 */
static VALUE
econv_inspect(VALUE self)
{
    const char *cname = rb_obj_classname(self);
    rb_econv_t *ec;

    TypedData_Get_Struct(self, rb_econv_t, &econv_data_type, ec);
    if (!ec)
        return rb_sprintf("#<%s: uninitialized>", cname);
    else {
        const char *sname = ec->source_encoding_name;
        const char *dname = ec->destination_encoding_name;
        VALUE str;
        str = rb_sprintf("#<%s: ", cname);
        econv_description(sname, dname, ec->flags, str);
        rb_str_cat2(str, ">");
        return str;
    }
}

static rb_econv_t *
check_econv(VALUE self)
{
    rb_econv_t *ec;

    TypedData_Get_Struct(self, rb_econv_t, &econv_data_type, ec);
    if (!ec) {
        rb_raise(rb_eTypeError, "uninitialized encoding converter");
    }
    return ec;
}

/*
 * call-seq:
 *   ec.source_encoding -> encoding
 *
 * Returns the source encoding as an Encoding object.
 */
static VALUE
econv_source_encoding(VALUE self)
{
    rb_econv_t *ec = check_econv(self);
    if (!ec->source_encoding)
        return Qnil;
    return rb_enc_from_encoding(ec->source_encoding);
}

/*
 * call-seq:
 *   ec.destination_encoding -> encoding
 *
 * Returns the destination encoding as an Encoding object.
 */
static VALUE
econv_destination_encoding(VALUE self)
{
    rb_econv_t *ec = check_econv(self);
    if (!ec->destination_encoding)
        return Qnil;
    return rb_enc_from_encoding(ec->destination_encoding);
}

/*
 * call-seq:
 *   ec.convpath        -> ary
 *
 * Returns the conversion path of ec.
 *
 * The result is an array of conversions.
 *
 *   ec = Encoding::Converter.new("ISO-8859-1", "EUC-JP", crlf_newline: true)
 *   p ec.convpath
 *   #=> [[#<Encoding:ISO-8859-1>, #<Encoding:UTF-8>],
 *   #    [#<Encoding:UTF-8>, #<Encoding:EUC-JP>],
 *   #    "crlf_newline"]
 *
 * Each element of the array is a pair of encodings or a string.
 * A pair means an encoding conversion.
 * A string means a decorator.
 *
 * In the above example, [#<Encoding:ISO-8859-1>, #<Encoding:UTF-8>] means
 * a converter from ISO-8859-1 to UTF-8.
 * "crlf_newline" means newline converter from LF to CRLF.
 */
static VALUE
econv_convpath(VALUE self)
{
    rb_econv_t *ec = check_econv(self);
    VALUE result;
    int i;

    result = rb_ary_new();
    for (i = 0; i < ec->num_trans; i++) {
        const rb_transcoder *tr = ec->elems[i].tc->transcoder;
        VALUE v;
        if (DECORATOR_P(tr->src_encoding, tr->dst_encoding))
            v = rb_str_new_cstr(tr->dst_encoding);
        else
            v = rb_assoc_new(make_encobj(tr->src_encoding), make_encobj(tr->dst_encoding));
        rb_ary_push(result, v);
    }
    return result;
}

/*
 * call-seq:
 *   ec == other        -> true or false
 */
static VALUE
econv_equal(VALUE self, VALUE other)
{
    rb_econv_t *ec1 = check_econv(self);
    rb_econv_t *ec2;
    int i;

    if (!rb_typeddata_is_kind_of(other, &econv_data_type)) {
	return Qnil;
    }
    ec2 = DATA_PTR(other);
    if (!ec2) return Qfalse;
    if (ec1->source_encoding_name != ec2->source_encoding_name &&
	strcmp(ec1->source_encoding_name, ec2->source_encoding_name))
	return Qfalse;
    if (ec1->destination_encoding_name != ec2->destination_encoding_name &&
	strcmp(ec1->destination_encoding_name, ec2->destination_encoding_name))
	return Qfalse;
    if (ec1->flags != ec2->flags) return Qfalse;
    if (ec1->replacement_enc != ec2->replacement_enc &&
	strcmp(ec1->replacement_enc, ec2->replacement_enc))
	return Qfalse;
    if (ec1->replacement_len != ec2->replacement_len) return Qfalse;
    if (ec1->replacement_str != ec2->replacement_str &&
	memcmp(ec1->replacement_str, ec2->replacement_str, ec2->replacement_len))
	return Qfalse;

    if (ec1->num_trans != ec2->num_trans) return Qfalse;
    for (i = 0; i < ec1->num_trans; i++) {
        if (ec1->elems[i].tc->transcoder != ec2->elems[i].tc->transcoder)
	    return Qfalse;
    }
    return Qtrue;
}

static VALUE
econv_result_to_symbol(rb_econv_result_t res)
{
    switch (res) {
      case econv_invalid_byte_sequence: return sym_invalid_byte_sequence;
      case econv_incomplete_input: return sym_incomplete_input;
      case econv_undefined_conversion: return sym_undefined_conversion;
      case econv_destination_buffer_full: return sym_destination_buffer_full;
      case econv_source_buffer_empty: return sym_source_buffer_empty;
      case econv_finished: return sym_finished;
      case econv_after_output: return sym_after_output;
      default: return INT2NUM(res); /* should not be reached */
    }
}

/*
 * call-seq:
 *   ec.primitive_convert(source_buffer, destination_buffer) -> symbol
 *   ec.primitive_convert(source_buffer, destination_buffer, destination_byteoffset) -> symbol
 *   ec.primitive_convert(source_buffer, destination_buffer, destination_byteoffset, destination_bytesize) -> symbol
 *   ec.primitive_convert(source_buffer, destination_buffer, destination_byteoffset, destination_bytesize, opt) -> symbol
 *
 * possible opt elements:
 *   hash form:
 *     :partial_input => true           # source buffer may be part of larger source
 *     :after_output => true            # stop conversion after output before input
 *   integer form:
 *     Encoding::Converter::PARTIAL_INPUT
 *     Encoding::Converter::AFTER_OUTPUT
 *
 * possible results:
 *    :invalid_byte_sequence
 *    :incomplete_input
 *    :undefined_conversion
 *    :after_output
 *    :destination_buffer_full
 *    :source_buffer_empty
 *    :finished
 *
 * primitive_convert converts source_buffer into destination_buffer.
 *
 * source_buffer should be a string or nil.
 * nil means an empty string.
 *
 * destination_buffer should be a string.
 *
 * destination_byteoffset should be an integer or nil.
 * nil means the end of destination_buffer.
 * If it is omitted, nil is assumed.
 *
 * destination_bytesize should be an integer or nil.
 * nil means unlimited.
 * If it is omitted, nil is assumed.
 *
 * opt should be nil, a hash or an integer.
 * nil means no flags.
 * If it is omitted, nil is assumed.
 *
 * primitive_convert converts the content of source_buffer from beginning
 * and store the result into destination_buffer.
 *
 * destination_byteoffset and destination_bytesize specify the region which
 * the converted result is stored.
 * destination_byteoffset specifies the start position in destination_buffer in bytes.
 * If destination_byteoffset is nil,
 * destination_buffer.bytesize is used for appending the result.
 * destination_bytesize specifies maximum number of bytes.
 * If destination_bytesize is nil,
 * destination size is unlimited.
 * After conversion, destination_buffer is resized to
 * destination_byteoffset + actually produced number of bytes.
 * Also destination_buffer's encoding is set to destination_encoding.
 *
 * primitive_convert drops the converted part of source_buffer.
 * the dropped part is converted in destination_buffer or
 * buffered in Encoding::Converter object.
 *
 * primitive_convert stops conversion when one of following condition met.
 * - invalid byte sequence found in source buffer (:invalid_byte_sequence)
 * - unexpected end of source buffer (:incomplete_input)
 *   this occur only when :partial_input is not specified.
 * - character not representable in output encoding (:undefined_conversion)
 * - after some output is generated, before input is done (:after_output)
 *   this occur only when :after_output is specified.
 * - destination buffer is full (:destination_buffer_full)
 *   this occur only when destination_bytesize is non-nil.
 * - source buffer is empty (:source_buffer_empty)
 *   this occur only when :partial_input is specified.
 * - conversion is finished (:finished)
 *
 * example:
 *   ec = Encoding::Converter.new("UTF-8", "UTF-16BE")
 *   ret = ec.primitive_convert(src="pi", dst="", nil, 100)
 *   p [ret, src, dst] #=> [:finished, "", "\x00p\x00i"]
 *
 *   ec = Encoding::Converter.new("UTF-8", "UTF-16BE")
 *   ret = ec.primitive_convert(src="pi", dst="", nil, 1)
 *   p [ret, src, dst] #=> [:destination_buffer_full, "i", "\x00"]
 *   ret = ec.primitive_convert(src, dst="", nil, 1)
 *   p [ret, src, dst] #=> [:destination_buffer_full, "", "p"]
 *   ret = ec.primitive_convert(src, dst="", nil, 1)
 *   p [ret, src, dst] #=> [:destination_buffer_full, "", "\x00"]
 *   ret = ec.primitive_convert(src, dst="", nil, 1)
 *   p [ret, src, dst] #=> [:finished, "", "i"]
 *
 */
static VALUE
econv_primitive_convert(int argc, VALUE *argv, VALUE self)
{
    VALUE input, output, output_byteoffset_v, output_bytesize_v, opt, flags_v;
    rb_econv_t *ec = check_econv(self);
    rb_econv_result_t res;
    const unsigned char *ip, *is;
    unsigned char *op, *os;
    long output_byteoffset, output_bytesize;
    unsigned long output_byteend;
    int flags;

    argc = rb_scan_args(argc, argv, "23:", &input, &output, &output_byteoffset_v, &output_bytesize_v, &flags_v, &opt);

    if (NIL_P(output_byteoffset_v))
        output_byteoffset = 0; /* dummy */
    else
        output_byteoffset = NUM2LONG(output_byteoffset_v);

    if (NIL_P(output_bytesize_v))
        output_bytesize = 0; /* dummy */
    else
        output_bytesize = NUM2LONG(output_bytesize_v);

    if (!NIL_P(flags_v)) {
	if (!NIL_P(opt)) {
	    rb_error_arity(argc + 1, 2, 5);
	}
	flags = NUM2INT(rb_to_int(flags_v));
    }
    else if (!NIL_P(opt)) {
        VALUE v;
        flags = 0;
        v = rb_hash_aref(opt, sym_partial_input);
        if (RTEST(v))
            flags |= ECONV_PARTIAL_INPUT;
        v = rb_hash_aref(opt, sym_after_output);
        if (RTEST(v))
            flags |= ECONV_AFTER_OUTPUT;
    }
    else {
        flags = 0;
    }

    StringValue(output);
    if (!NIL_P(input))
        StringValue(input);
    rb_str_modify(output);

    if (NIL_P(output_bytesize_v)) {
        output_bytesize = RSTRING_EMBED_LEN_MAX;
        if (!NIL_P(input) && output_bytesize < RSTRING_LEN(input))
            output_bytesize = RSTRING_LEN(input);
    }

  retry:

    if (NIL_P(output_byteoffset_v))
        output_byteoffset = RSTRING_LEN(output);

    if (output_byteoffset < 0)
        rb_raise(rb_eArgError, "negative output_byteoffset");

    if (RSTRING_LEN(output) < output_byteoffset)
        rb_raise(rb_eArgError, "output_byteoffset too big");

    if (output_bytesize < 0)
        rb_raise(rb_eArgError, "negative output_bytesize");

    output_byteend = (unsigned long)output_byteoffset +
                     (unsigned long)output_bytesize;

    if (output_byteend < (unsigned long)output_byteoffset ||
        LONG_MAX < output_byteend)
        rb_raise(rb_eArgError, "output_byteoffset+output_bytesize too big");

    if (rb_str_capacity(output) < output_byteend)
        rb_str_resize(output, output_byteend);

    if (NIL_P(input)) {
        ip = is = NULL;
    }
    else {
        ip = (const unsigned char *)RSTRING_PTR(input);
        is = ip + RSTRING_LEN(input);
    }

    op = (unsigned char *)RSTRING_PTR(output) + output_byteoffset;
    os = op + output_bytesize;

    res = rb_econv_convert(ec, &ip, is, &op, os, flags);
    rb_str_set_len(output, op-(unsigned char *)RSTRING_PTR(output));
    if (!NIL_P(input))
        rb_str_drop_bytes(input, ip - (unsigned char *)RSTRING_PTR(input));

    if (NIL_P(output_bytesize_v) && res == econv_destination_buffer_full) {
        if (LONG_MAX / 2 < output_bytesize)
            rb_raise(rb_eArgError, "too long conversion result");
        output_bytesize *= 2;
        output_byteoffset_v = Qnil;
        goto retry;
    }

    if (ec->destination_encoding) {
        rb_enc_associate(output, ec->destination_encoding);
    }

    return econv_result_to_symbol(res);
}

/*
 * call-seq:
 *   ec.convert(source_string) -> destination_string
 *
 * Convert source_string and return destination_string.
 *
 * source_string is assumed as a part of source.
 * i.e.  :partial_input=>true is specified internally.
 * finish method should be used last.
 *
 *   ec = Encoding::Converter.new("utf-8", "euc-jp")
 *   puts ec.convert("\u3042").dump     #=> "\xA4\xA2"
 *   puts ec.finish.dump                #=> ""
 *
 *   ec = Encoding::Converter.new("euc-jp", "utf-8")
 *   puts ec.convert("\xA4").dump       #=> ""
 *   puts ec.convert("\xA2").dump       #=> "\xE3\x81\x82"
 *   puts ec.finish.dump                #=> ""
 *
 *   ec = Encoding::Converter.new("utf-8", "iso-2022-jp")
 *   puts ec.convert("\xE3").dump       #=> "".force_encoding("ISO-2022-JP")
 *   puts ec.convert("\x81").dump       #=> "".force_encoding("ISO-2022-JP")
 *   puts ec.convert("\x82").dump       #=> "\e$B$\"".force_encoding("ISO-2022-JP")
 *   puts ec.finish.dump                #=> "\e(B".force_encoding("ISO-2022-JP")
 *
 * If a conversion error occur,
 * Encoding::UndefinedConversionError or
 * Encoding::InvalidByteSequenceError is raised.
 * Encoding::Converter#convert doesn't supply methods to recover or restart
 * from these exceptions.
 * When you want to handle these conversion errors,
 * use Encoding::Converter#primitive_convert.
 *
 */
static VALUE
econv_convert(VALUE self, VALUE source_string)
{
    VALUE ret, dst;
    VALUE av[5];
    int ac;
    rb_econv_t *ec = check_econv(self);

    StringValue(source_string);

    dst = rb_str_new(NULL, 0);

    av[0] = rb_str_dup(source_string);
    av[1] = dst;
    av[2] = Qnil;
    av[3] = Qnil;
    av[4] = INT2NUM(ECONV_PARTIAL_INPUT);
    ac = 5;

    ret = econv_primitive_convert(ac, av, self);

    if (ret == sym_invalid_byte_sequence ||
        ret == sym_undefined_conversion ||
        ret == sym_incomplete_input) {
        VALUE exc = make_econv_exception(ec);
        rb_exc_raise(exc);
    }

    if (ret == sym_finished) {
        rb_raise(rb_eArgError, "converter already finished");
    }

    if (ret != sym_source_buffer_empty) {
        rb_bug("unexpected result of econv_primitive_convert");
    }

    return dst;
}

/*
 * call-seq:
 *   ec.finish -> string
 *
 * Finishes the converter.
 * It returns the last part of the converted string.
 *
 *   ec = Encoding::Converter.new("utf-8", "iso-2022-jp")
 *   p ec.convert("\u3042")     #=> "\e$B$\""
 *   p ec.finish                #=> "\e(B"
 */
static VALUE
econv_finish(VALUE self)
{
    VALUE ret, dst;
    VALUE av[5];
    int ac;
    rb_econv_t *ec = check_econv(self);

    dst = rb_str_new(NULL, 0);

    av[0] = Qnil;
    av[1] = dst;
    av[2] = Qnil;
    av[3] = Qnil;
    av[4] = INT2NUM(0);
    ac = 5;

    ret = econv_primitive_convert(ac, av, self);

    if (ret == sym_invalid_byte_sequence ||
        ret == sym_undefined_conversion ||
        ret == sym_incomplete_input) {
        VALUE exc = make_econv_exception(ec);
        rb_exc_raise(exc);
    }

    if (ret != sym_finished) {
        rb_bug("unexpected result of econv_primitive_convert");
    }

    return dst;
}

/*
 * call-seq:
 *   ec.primitive_errinfo -> array
 *
 * primitive_errinfo returns important information regarding the last error
 * as a 5-element array:
 *
 *   [result, enc1, enc2, error_bytes, readagain_bytes]
 *
 * result is the last result of primitive_convert.
 *
 * Other elements are only meaningful when result is
 * :invalid_byte_sequence, :incomplete_input or :undefined_conversion.
 *
 * enc1 and enc2 indicate a conversion step as a pair of strings.
 * For example, a converter from EUC-JP to ISO-8859-1 converts
 * a string as follows: EUC-JP -> UTF-8 -> ISO-8859-1.
 * So [enc1, enc2] is either ["EUC-JP", "UTF-8"] or ["UTF-8", "ISO-8859-1"].
 *
 * error_bytes and readagain_bytes indicate the byte sequences which caused the error.
 * error_bytes is discarded portion.
 * readagain_bytes is buffered portion which is read again on next conversion.
 *
 * Example:
 *
 *   # \xff is invalid as EUC-JP.
 *   ec = Encoding::Converter.new("EUC-JP", "Shift_JIS")
 *   ec.primitive_convert(src="\xff", dst="", nil, 10)
 *   p ec.primitive_errinfo
 *   #=> [:invalid_byte_sequence, "EUC-JP", "UTF-8", "\xFF", ""]
 *
 *   # HIRAGANA LETTER A (\xa4\xa2 in EUC-JP) is not representable in ISO-8859-1.
 *   # Since this error is occur in UTF-8 to ISO-8859-1 conversion,
 *   # error_bytes is HIRAGANA LETTER A in UTF-8 (\xE3\x81\x82).
 *   ec = Encoding::Converter.new("EUC-JP", "ISO-8859-1")
 *   ec.primitive_convert(src="\xa4\xa2", dst="", nil, 10)
 *   p ec.primitive_errinfo
 *   #=> [:undefined_conversion, "UTF-8", "ISO-8859-1", "\xE3\x81\x82", ""]
 *
 *   # partial character is invalid
 *   ec = Encoding::Converter.new("EUC-JP", "ISO-8859-1")
 *   ec.primitive_convert(src="\xa4", dst="", nil, 10)
 *   p ec.primitive_errinfo
 *   #=> [:incomplete_input, "EUC-JP", "UTF-8", "\xA4", ""]
 *
 *   # Encoding::Converter::PARTIAL_INPUT prevents invalid errors by
 *   # partial characters.
 *   ec = Encoding::Converter.new("EUC-JP", "ISO-8859-1")
 *   ec.primitive_convert(src="\xa4", dst="", nil, 10, Encoding::Converter::PARTIAL_INPUT)
 *   p ec.primitive_errinfo
 *   #=> [:source_buffer_empty, nil, nil, nil, nil]
 *
 *   # \xd8\x00\x00@ is invalid as UTF-16BE because
 *   # no low surrogate after high surrogate (\xd8\x00).
 *   # It is detected by 3rd byte (\00) which is part of next character.
 *   # So the high surrogate (\xd8\x00) is discarded and
 *   # the 3rd byte is read again later.
 *   # Since the byte is buffered in ec, it is dropped from src.
 *   ec = Encoding::Converter.new("UTF-16BE", "UTF-8")
 *   ec.primitive_convert(src="\xd8\x00\x00@", dst="", nil, 10)
 *   p ec.primitive_errinfo
 *   #=> [:invalid_byte_sequence, "UTF-16BE", "UTF-8", "\xD8\x00", "\x00"]
 *   p src
 *   #=> "@"
 *
 *   # Similar to UTF-16BE, \x00\xd8@\x00 is invalid as UTF-16LE.
 *   # The problem is detected by 4th byte.
 *   ec = Encoding::Converter.new("UTF-16LE", "UTF-8")
 *   ec.primitive_convert(src="\x00\xd8@\x00", dst="", nil, 10)
 *   p ec.primitive_errinfo
 *   #=> [:invalid_byte_sequence, "UTF-16LE", "UTF-8", "\x00\xD8", "@\x00"]
 *   p src
 *   #=> ""
 *
 */
static VALUE
econv_primitive_errinfo(VALUE self)
{
    rb_econv_t *ec = check_econv(self);

    VALUE ary;

    ary = rb_ary_new2(5);

    rb_ary_store(ary, 0, econv_result_to_symbol(ec->last_error.result));
    rb_ary_store(ary, 4, Qnil);

    if (ec->last_error.source_encoding)
        rb_ary_store(ary, 1, rb_str_new2(ec->last_error.source_encoding));

    if (ec->last_error.destination_encoding)
        rb_ary_store(ary, 2, rb_str_new2(ec->last_error.destination_encoding));

    if (ec->last_error.error_bytes_start) {
        rb_ary_store(ary, 3, rb_str_new((const char *)ec->last_error.error_bytes_start, ec->last_error.error_bytes_len));
        rb_ary_store(ary, 4, rb_str_new((const char *)ec->last_error.error_bytes_start + ec->last_error.error_bytes_len, ec->last_error.readagain_len));
    }

    return ary;
}

/*
 * call-seq:
 *   ec.insert_output(string) -> nil
 *
 * Inserts string into the encoding converter.
 * The string will be converted to the destination encoding and
 * output on later conversions.
 *
 * If the destination encoding is stateful,
 * string is converted according to the state and the state is updated.
 *
 * This method should be used only when a conversion error occurs.
 *
 *  ec = Encoding::Converter.new("utf-8", "iso-8859-1")
 *  src = "HIRAGANA LETTER A is \u{3042}."
 *  dst = ""
 *  p ec.primitive_convert(src, dst)    #=> :undefined_conversion
 *  puts "[#{dst.dump}, #{src.dump}]"   #=> ["HIRAGANA LETTER A is ", "."]
 *  ec.insert_output("<err>")
 *  p ec.primitive_convert(src, dst)    #=> :finished
 *  puts "[#{dst.dump}, #{src.dump}]"   #=> ["HIRAGANA LETTER A is <err>.", ""]
 *
 *  ec = Encoding::Converter.new("utf-8", "iso-2022-jp")
 *  src = "\u{306F 3041 3068 2661 3002}" # U+2661 is not representable in iso-2022-jp
 *  dst = ""
 *  p ec.primitive_convert(src, dst)    #=> :undefined_conversion
 *  puts "[#{dst.dump}, #{src.dump}]"   #=> ["\e$B$O$!$H".force_encoding("ISO-2022-JP"), "\xE3\x80\x82"]
 *  ec.insert_output "?"                # state change required to output "?".
 *  p ec.primitive_convert(src, dst)    #=> :finished
 *  puts "[#{dst.dump}, #{src.dump}]"   #=> ["\e$B$O$!$H\e(B?\e$B!#\e(B".force_encoding("ISO-2022-JP"), ""]
 *
 */
static VALUE
econv_insert_output(VALUE self, VALUE string)
{
    const char *insert_enc;

    int ret;

    rb_econv_t *ec = check_econv(self);

    StringValue(string);
    insert_enc = rb_econv_encoding_to_insert_output(ec);
    string = rb_str_encode(string, rb_enc_from_encoding(rb_enc_find(insert_enc)), 0, Qnil);

    ret = rb_econv_insert_output(ec, (const unsigned char *)RSTRING_PTR(string), RSTRING_LEN(string), insert_enc);
    if (ret == -1) {
	rb_raise(rb_eArgError, "too big string");
    }

    return Qnil;
}

/*
 * call-seq
 *   ec.putback                    -> string
 *   ec.putback(max_numbytes)      -> string
 *
 * Put back the bytes which will be converted.
 *
 * The bytes are caused by invalid_byte_sequence error.
 * When invalid_byte_sequence error, some bytes are discarded and
 * some bytes are buffered to be converted later.
 * The latter bytes can be put back.
 * It can be observed by
 * Encoding::InvalidByteSequenceError#readagain_bytes and
 * Encoding::Converter#primitive_errinfo.
 *
 *   ec = Encoding::Converter.new("utf-16le", "iso-8859-1")
 *   src = "\x00\xd8\x61\x00"
 *   dst = ""
 *   p ec.primitive_convert(src, dst)   #=> :invalid_byte_sequence
 *   p ec.primitive_errinfo     #=> [:invalid_byte_sequence, "UTF-16LE", "UTF-8", "\x00\xD8", "a\x00"]
 *   p ec.putback               #=> "a\x00"
 *   p ec.putback               #=> ""          # no more bytes to put back
 *
 */
static VALUE
econv_putback(int argc, VALUE *argv, VALUE self)
{
    rb_econv_t *ec = check_econv(self);
    int n;
    int putbackable;
    VALUE str, max;

    rb_scan_args(argc, argv, "01", &max);

    if (NIL_P(max))
        n = rb_econv_putbackable(ec);
    else {
        n = NUM2INT(max);
        putbackable = rb_econv_putbackable(ec);
        if (putbackable < n)
            n = putbackable;
    }

    str = rb_str_new(NULL, n);
    rb_econv_putback(ec, (unsigned char *)RSTRING_PTR(str), n);

    if (ec->source_encoding) {
        rb_enc_associate(str, ec->source_encoding);
    }

    return str;
}

/*
 * call-seq:
 *   ec.last_error -> exception or nil
 *
 * Returns an exception object for the last conversion.
 * Returns nil if the last conversion did not produce an error.
 *
 * "error" means that
 * Encoding::InvalidByteSequenceError and Encoding::UndefinedConversionError for
 * Encoding::Converter#convert and
 * :invalid_byte_sequence, :incomplete_input and :undefined_conversion for
 * Encoding::Converter#primitive_convert.
 *
 *  ec = Encoding::Converter.new("utf-8", "iso-8859-1")
 *  p ec.primitive_convert(src="\xf1abcd", dst="")       #=> :invalid_byte_sequence
 *  p ec.last_error      #=> #<Encoding::InvalidByteSequenceError: "\xF1" followed by "a" on UTF-8>
 *  p ec.primitive_convert(src, dst, nil, 1)             #=> :destination_buffer_full
 *  p ec.last_error      #=> nil
 *
 */
static VALUE
econv_last_error(VALUE self)
{
    rb_econv_t *ec = check_econv(self);
    VALUE exc;

    exc = make_econv_exception(ec);
    if (NIL_P(exc))
        return Qnil;
    return exc;
}

/*
 * call-seq:
 *   ec.replacement -> string
 *
 * Returns the replacement string.
 *
 *  ec = Encoding::Converter.new("euc-jp", "us-ascii")
 *  p ec.replacement    #=> "?"
 *
 *  ec = Encoding::Converter.new("euc-jp", "utf-8")
 *  p ec.replacement    #=> "\uFFFD"
 */
static VALUE
econv_get_replacement(VALUE self)
{
    rb_econv_t *ec = check_econv(self);
    int ret;
    rb_encoding *enc;

    ret = make_replacement(ec);
    if (ret == -1) {
        rb_raise(rb_eUndefinedConversionError, "replacement character setup failed");
    }

    enc = rb_enc_find(ec->replacement_enc);
    return rb_enc_str_new((const char *)ec->replacement_str, (long)ec->replacement_len, enc);
}

/*
 * call-seq:
 *   ec.replacement = string
 *
 * Sets the replacement string.
 *
 *  ec = Encoding::Converter.new("utf-8", "us-ascii", :undef => :replace)
 *  ec.replacement = "<undef>"
 *  p ec.convert("a \u3042 b")      #=> "a <undef> b"
 */
static VALUE
econv_set_replacement(VALUE self, VALUE arg)
{
    rb_econv_t *ec = check_econv(self);
    VALUE string = arg;
    int ret;
    rb_encoding *enc;

    StringValue(string);
    enc = rb_enc_get(string);

    ret = rb_econv_set_replacement(ec,
            (const unsigned char *)RSTRING_PTR(string),
            RSTRING_LEN(string),
            rb_enc_name(enc));

    if (ret == -1) {
        /* xxx: rb_eInvalidByteSequenceError? */
        rb_raise(rb_eUndefinedConversionError, "replacement character setup failed");
    }

    return arg;
}

VALUE
rb_econv_make_exception(rb_econv_t *ec)
{
    return make_econv_exception(ec);
}

void
rb_econv_check_error(rb_econv_t *ec)
{
    VALUE exc;

    exc = make_econv_exception(ec);
    if (NIL_P(exc))
        return;
    rb_exc_raise(exc);
}

/*
 * call-seq:
 *   ecerr.source_encoding_name         -> string
 *
 * Returns the source encoding name as a string.
 */
static VALUE
ecerr_source_encoding_name(VALUE self)
{
    return rb_attr_get(self, rb_intern("source_encoding_name"));
}

/*
 * call-seq:
 *   ecerr.source_encoding              -> encoding
 *
 * Returns the source encoding as an encoding object.
 *
 * Note that the result may not be equal to the source encoding of
 * the encoding converter if the conversion has multiple steps.
 *
 *  ec = Encoding::Converter.new("ISO-8859-1", "EUC-JP") # ISO-8859-1 -> UTF-8 -> EUC-JP
 *  begin
 *    ec.convert("\xa0") # NO-BREAK SPACE, which is available in UTF-8 but not in EUC-JP.
 *  rescue Encoding::UndefinedConversionError
 *    p $!.source_encoding              #=> #<Encoding:UTF-8>
 *    p $!.destination_encoding         #=> #<Encoding:EUC-JP>
 *    p $!.source_encoding_name         #=> "UTF-8"
 *    p $!.destination_encoding_name    #=> "EUC-JP"
 *  end
 *
 */
static VALUE
ecerr_source_encoding(VALUE self)
{
    return rb_attr_get(self, rb_intern("source_encoding"));
}

/*
 * call-seq:
 *   ecerr.destination_encoding_name         -> string
 *
 * Returns the destination encoding name as a string.
 */
static VALUE
ecerr_destination_encoding_name(VALUE self)
{
    return rb_attr_get(self, rb_intern("destination_encoding_name"));
}

/*
 * call-seq:
 *   ecerr.destination_encoding         -> string
 *
 * Returns the destination encoding as an encoding object.
 */
static VALUE
ecerr_destination_encoding(VALUE self)
{
    return rb_attr_get(self, rb_intern("destination_encoding"));
}

/*
 * call-seq:
 *   ecerr.error_char         -> string
 *
 * Returns the one-character string which cause Encoding::UndefinedConversionError.
 *
 *  ec = Encoding::Converter.new("ISO-8859-1", "EUC-JP")
 *  begin
 *    ec.convert("\xa0")
 *  rescue Encoding::UndefinedConversionError
 *    puts $!.error_char.dump   #=> "\xC2\xA0"
 *    p $!.error_char.encoding  #=> #<Encoding:UTF-8>
 *  end
 *
 */
static VALUE
ecerr_error_char(VALUE self)
{
    return rb_attr_get(self, rb_intern("error_char"));
}

/*
 * call-seq:
 *   ecerr.error_bytes         -> string
 *
 * Returns the discarded bytes when Encoding::InvalidByteSequenceError occurs.
 *
 *  ec = Encoding::Converter.new("EUC-JP", "ISO-8859-1")
 *  begin
 *    ec.convert("abc\xA1\xFFdef")
 *  rescue Encoding::InvalidByteSequenceError
 *    p $!      #=> #<Encoding::InvalidByteSequenceError: "\xA1" followed by "\xFF" on EUC-JP>
 *    puts $!.error_bytes.dump          #=> "\xA1"
 *    puts $!.readagain_bytes.dump      #=> "\xFF"
 *  end
 */
static VALUE
ecerr_error_bytes(VALUE self)
{
    return rb_attr_get(self, rb_intern("error_bytes"));
}

/*
 * call-seq:
 *   ecerr.readagain_bytes         -> string
 *
 * Returns the bytes to be read again when Encoding::InvalidByteSequenceError occurs.
 */
static VALUE
ecerr_readagain_bytes(VALUE self)
{
    return rb_attr_get(self, rb_intern("readagain_bytes"));
}

/*
 * call-seq:
 *   ecerr.incomplete_input?         -> true or false
 *
 * Returns true if the invalid byte sequence error is caused by
 * premature end of string.
 *
 *  ec = Encoding::Converter.new("EUC-JP", "ISO-8859-1")
 *
 *  begin
 *    ec.convert("abc\xA1z")
 *  rescue Encoding::InvalidByteSequenceError
 *    p $!      #=> #<Encoding::InvalidByteSequenceError: "\xA1" followed by "z" on EUC-JP>
 *    p $!.incomplete_input?    #=> false
 *  end
 *
 *  begin
 *    ec.convert("abc\xA1")
 *    ec.finish
 *  rescue Encoding::InvalidByteSequenceError
 *    p $!      #=> #<Encoding::InvalidByteSequenceError: incomplete "\xA1" on EUC-JP>
 *    p $!.incomplete_input?    #=> true
 *  end
 */
static VALUE
ecerr_incomplete_input(VALUE self)
{
    return rb_attr_get(self, rb_intern("incomplete_input"));
}

/*
 *  Document-class: Encoding::UndefinedConversionError
 *
 *  Raised by Encoding and String methods when a transcoding operation
 *  fails.
 */

/*
 *  Document-class: Encoding::InvalidByteSequenceError
 *
 *  Raised by Encoding and String methods when the string being
 *  transcoded contains a byte invalid for the either the source or
 *  target encoding.
 */

/*
 *  Document-class: Encoding::ConverterNotFoundError
 *
 *  Raised by transcoding methods when a named encoding does not
 *  correspond with a known converter.
 */

void
Init_transcode(void)
{
    rb_eUndefinedConversionError = rb_define_class_under(rb_cEncoding, "UndefinedConversionError", rb_eEncodingError);
    rb_eInvalidByteSequenceError = rb_define_class_under(rb_cEncoding, "InvalidByteSequenceError", rb_eEncodingError);
    rb_eConverterNotFoundError = rb_define_class_under(rb_cEncoding, "ConverterNotFoundError", rb_eEncodingError);

    transcoder_table = st_init_strcasetable();

    sym_invalid = ID2SYM(rb_intern("invalid"));
    sym_undef = ID2SYM(rb_intern("undef"));
    sym_replace = ID2SYM(rb_intern("replace"));
    sym_fallback = ID2SYM(rb_intern("fallback"));
    sym_aref = ID2SYM(rb_intern("[]"));
    sym_xml = ID2SYM(rb_intern("xml"));
    sym_text = ID2SYM(rb_intern("text"));
    sym_attr = ID2SYM(rb_intern("attr"));

    sym_invalid_byte_sequence = ID2SYM(rb_intern("invalid_byte_sequence"));
    sym_undefined_conversion = ID2SYM(rb_intern("undefined_conversion"));
    sym_destination_buffer_full = ID2SYM(rb_intern("destination_buffer_full"));
    sym_source_buffer_empty = ID2SYM(rb_intern("source_buffer_empty"));
    sym_finished = ID2SYM(rb_intern("finished"));
    sym_after_output = ID2SYM(rb_intern("after_output"));
    sym_incomplete_input = ID2SYM(rb_intern("incomplete_input"));
    sym_universal_newline = ID2SYM(rb_intern("universal_newline"));
    sym_crlf_newline = ID2SYM(rb_intern("crlf_newline"));
    sym_cr_newline = ID2SYM(rb_intern("cr_newline"));
    sym_partial_input = ID2SYM(rb_intern("partial_input"));

#ifdef ENABLE_ECONV_NEWLINE_OPTION
    sym_newline = ID2SYM(rb_intern("newline"));
    sym_universal = ID2SYM(rb_intern("universal"));
    sym_crlf = ID2SYM(rb_intern("crlf"));
    sym_cr = ID2SYM(rb_intern("cr"));
    sym_lf = ID2SYM(rb_intern("lf"));
#endif

    rb_define_method(rb_cString, "encode", str_encode, -1);
    rb_define_method(rb_cString, "encode!", str_encode_bang, -1);

    rb_cEncodingConverter = rb_define_class_under(rb_cEncoding, "Converter", rb_cData);
    rb_define_alloc_func(rb_cEncodingConverter, econv_s_allocate);
    rb_define_singleton_method(rb_cEncodingConverter, "asciicompat_encoding", econv_s_asciicompat_encoding, 1);
    rb_define_singleton_method(rb_cEncodingConverter, "search_convpath", econv_s_search_convpath, -1);
    rb_define_method(rb_cEncodingConverter, "initialize", econv_init, -1);
    rb_define_method(rb_cEncodingConverter, "inspect", econv_inspect, 0);
    rb_define_method(rb_cEncodingConverter, "convpath", econv_convpath, 0);
    rb_define_method(rb_cEncodingConverter, "source_encoding", econv_source_encoding, 0);
    rb_define_method(rb_cEncodingConverter, "destination_encoding", econv_destination_encoding, 0);
    rb_define_method(rb_cEncodingConverter, "primitive_convert", econv_primitive_convert, -1);
    rb_define_method(rb_cEncodingConverter, "convert", econv_convert, 1);
    rb_define_method(rb_cEncodingConverter, "finish", econv_finish, 0);
    rb_define_method(rb_cEncodingConverter, "primitive_errinfo", econv_primitive_errinfo, 0);
    rb_define_method(rb_cEncodingConverter, "insert_output", econv_insert_output, 1);
    rb_define_method(rb_cEncodingConverter, "putback", econv_putback, -1);
    rb_define_method(rb_cEncodingConverter, "last_error", econv_last_error, 0);
    rb_define_method(rb_cEncodingConverter, "replacement", econv_get_replacement, 0);
    rb_define_method(rb_cEncodingConverter, "replacement=", econv_set_replacement, 1);
    rb_define_method(rb_cEncodingConverter, "==", econv_equal, 1);

    rb_define_const(rb_cEncodingConverter, "INVALID_MASK", INT2FIX(ECONV_INVALID_MASK));
    rb_define_const(rb_cEncodingConverter, "INVALID_REPLACE", INT2FIX(ECONV_INVALID_REPLACE));
    rb_define_const(rb_cEncodingConverter, "UNDEF_MASK", INT2FIX(ECONV_UNDEF_MASK));
    rb_define_const(rb_cEncodingConverter, "UNDEF_REPLACE", INT2FIX(ECONV_UNDEF_REPLACE));
    rb_define_const(rb_cEncodingConverter, "UNDEF_HEX_CHARREF", INT2FIX(ECONV_UNDEF_HEX_CHARREF));
    rb_define_const(rb_cEncodingConverter, "PARTIAL_INPUT", INT2FIX(ECONV_PARTIAL_INPUT));
    rb_define_const(rb_cEncodingConverter, "AFTER_OUTPUT", INT2FIX(ECONV_AFTER_OUTPUT));
    rb_define_const(rb_cEncodingConverter, "UNIVERSAL_NEWLINE_DECORATOR", INT2FIX(ECONV_UNIVERSAL_NEWLINE_DECORATOR));
    rb_define_const(rb_cEncodingConverter, "CRLF_NEWLINE_DECORATOR", INT2FIX(ECONV_CRLF_NEWLINE_DECORATOR));
    rb_define_const(rb_cEncodingConverter, "CR_NEWLINE_DECORATOR", INT2FIX(ECONV_CR_NEWLINE_DECORATOR));
    rb_define_const(rb_cEncodingConverter, "XML_TEXT_DECORATOR", INT2FIX(ECONV_XML_TEXT_DECORATOR));
    rb_define_const(rb_cEncodingConverter, "XML_ATTR_CONTENT_DECORATOR", INT2FIX(ECONV_XML_ATTR_CONTENT_DECORATOR));
    rb_define_const(rb_cEncodingConverter, "XML_ATTR_QUOTE_DECORATOR", INT2FIX(ECONV_XML_ATTR_QUOTE_DECORATOR));

    rb_define_method(rb_eUndefinedConversionError, "source_encoding_name", ecerr_source_encoding_name, 0);
    rb_define_method(rb_eUndefinedConversionError, "destination_encoding_name", ecerr_destination_encoding_name, 0);
    rb_define_method(rb_eUndefinedConversionError, "source_encoding", ecerr_source_encoding, 0);
    rb_define_method(rb_eUndefinedConversionError, "destination_encoding", ecerr_destination_encoding, 0);
    rb_define_method(rb_eUndefinedConversionError, "error_char", ecerr_error_char, 0);

    rb_define_method(rb_eInvalidByteSequenceError, "source_encoding_name", ecerr_source_encoding_name, 0);
    rb_define_method(rb_eInvalidByteSequenceError, "destination_encoding_name", ecerr_destination_encoding_name, 0);
    rb_define_method(rb_eInvalidByteSequenceError, "source_encoding", ecerr_source_encoding, 0);
    rb_define_method(rb_eInvalidByteSequenceError, "destination_encoding", ecerr_destination_encoding, 0);
    rb_define_method(rb_eInvalidByteSequenceError, "error_bytes", ecerr_error_bytes, 0);
    rb_define_method(rb_eInvalidByteSequenceError, "readagain_bytes", ecerr_readagain_bytes, 0);
    rb_define_method(rb_eInvalidByteSequenceError, "incomplete_input?", ecerr_incomplete_input, 0);

    Init_newline();
}
