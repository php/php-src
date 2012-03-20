#include <psych.h>

VALUE cPsychParser;
VALUE ePsychSyntaxError;

static ID id_read;
static ID id_path;
static ID id_empty;
static ID id_start_stream;
static ID id_end_stream;
static ID id_start_document;
static ID id_end_document;
static ID id_alias;
static ID id_scalar;
static ID id_start_sequence;
static ID id_end_sequence;
static ID id_start_mapping;
static ID id_end_mapping;

#define PSYCH_TRANSCODE(_str, _yaml_enc, _internal_enc) \
  do { \
    rb_enc_associate_index((_str), (_yaml_enc)); \
    if(_internal_enc) \
      (_str) = rb_str_export_to_enc((_str), (_internal_enc)); \
  } while (0)

static int io_reader(void * data, unsigned char *buf, size_t size, size_t *read)
{
    VALUE io = (VALUE)data;
    VALUE string = rb_funcall(io, id_read, 1, INT2NUM(size));

    *read = 0;

    if(! NIL_P(string)) {
	void * str = (void *)StringValuePtr(string);
	*read = (size_t)RSTRING_LEN(string);
	memcpy(buf, str, *read);
    }

    return 1;
}

static void dealloc(void * ptr)
{
    yaml_parser_t * parser;

    parser = (yaml_parser_t *)ptr;
    yaml_parser_delete(parser);
    xfree(parser);
}

static VALUE allocate(VALUE klass)
{
    yaml_parser_t * parser;

    parser = xmalloc(sizeof(yaml_parser_t));
    yaml_parser_initialize(parser);

    return Data_Wrap_Struct(klass, 0, dealloc, parser);
}

static VALUE make_exception(yaml_parser_t * parser, VALUE path)
{
    size_t line, column;

    line = parser->context_mark.line + 1;
    column = parser->context_mark.column + 1;

    return rb_funcall(ePsychSyntaxError, rb_intern("new"), 6,
	    path,
	    INT2NUM(line),
	    INT2NUM(column),
	    INT2NUM(parser->problem_offset),
	    parser->problem ? rb_usascii_str_new2(parser->problem) : Qnil,
	    parser->context ? rb_usascii_str_new2(parser->context) : Qnil);
}

#ifdef HAVE_RUBY_ENCODING_H
static VALUE transcode_string(VALUE src, int * parser_encoding)
{
    int utf8    = rb_utf8_encindex();
    int utf16le = rb_enc_find_index("UTF16_LE");
    int utf16be = rb_enc_find_index("UTF16_BE");
    int source_encoding = rb_enc_get_index(src);

    if (source_encoding == utf8) {
	*parser_encoding = YAML_UTF8_ENCODING;
	return src;
    }

    if (source_encoding == utf16le) {
	*parser_encoding = YAML_UTF16LE_ENCODING;
	return src;
    }

    if (source_encoding == utf16be) {
	*parser_encoding = YAML_UTF16BE_ENCODING;
	return src;
    }

    src = rb_str_export_to_enc(src, rb_utf8_encoding());
    RB_GC_GUARD(src);

    *parser_encoding = YAML_UTF8_ENCODING;
    return src;
}

static VALUE transcode_io(VALUE src, int * parser_encoding)
{
    VALUE io_external_encoding;
    int io_external_enc_index;

    io_external_encoding = rb_funcall(src, rb_intern("external_encoding"), 0);

    /* if no encoding is returned, assume ascii8bit. */
    if (NIL_P(io_external_encoding)) {
	io_external_enc_index = rb_ascii8bit_encindex();
    } else {
	io_external_enc_index = rb_to_encoding_index(io_external_encoding);
    }

    /* Treat US-ASCII as utf_8 */
    if (io_external_enc_index == rb_usascii_encindex()) {
	*parser_encoding = YAML_UTF8_ENCODING;
	return src;
    }

    if (io_external_enc_index == rb_utf8_encindex()) {
	*parser_encoding = YAML_UTF8_ENCODING;
	return src;
    }

    if (io_external_enc_index == rb_enc_find_index("UTF-16LE")) {
	*parser_encoding = YAML_UTF16LE_ENCODING;
	return src;
    }

    if (io_external_enc_index == rb_enc_find_index("UTF-16BE")) {
	*parser_encoding = YAML_UTF16BE_ENCODING;
	return src;
    }

    /* Just guess on ASCII-8BIT */
    if (io_external_enc_index == rb_ascii8bit_encindex()) {
	*parser_encoding = YAML_ANY_ENCODING;
	return src;
    }

    rb_raise(rb_eArgError, "YAML file must be UTF-8, UTF-16LE, or UTF-16BE, not %s",
	    rb_enc_name(rb_enc_from_index(io_external_enc_index)));

    return Qnil;
}

#endif

static VALUE protected_start_stream(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall(args[0], id_start_stream, 1, args[1]);
}

static VALUE protected_start_document(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall3(args[0], id_start_document, 3, args + 1);
}

static VALUE protected_end_document(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall(args[0], id_end_document, 1, args[1]);
}

static VALUE protected_alias(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall(args[0], id_alias, 1, args[1]);
}

static VALUE protected_scalar(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall3(args[0], id_scalar, 6, args + 1);
}

static VALUE protected_start_sequence(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall3(args[0], id_start_sequence, 4, args + 1);
}

static VALUE protected_end_sequence(VALUE handler)
{
    return rb_funcall(handler, id_end_sequence, 0);
}

static VALUE protected_start_mapping(VALUE pointer)
{
    VALUE *args = (VALUE *)pointer;
    return rb_funcall3(args[0], id_start_mapping, 4, args + 1);
}

static VALUE protected_end_mapping(VALUE handler)
{
    return rb_funcall(handler, id_end_mapping, 0);
}

static VALUE protected_empty(VALUE handler)
{
    return rb_funcall(handler, id_empty, 0);
}

static VALUE protected_end_stream(VALUE handler)
{
    return rb_funcall(handler, id_end_stream, 0);
}

/*
 * call-seq:
 *    parser.parse(yaml)
 *
 * Parse the YAML document contained in +yaml+.  Events will be called on
 * the handler set on the parser instance.
 *
 * See Psych::Parser and Psych::Parser#handler
 */
static VALUE parse(int argc, VALUE *argv, VALUE self)
{
    VALUE yaml, path;
    yaml_parser_t * parser;
    yaml_event_t event;
    int done = 0;
    int tainted = 0;
    int state = 0;
    int parser_encoding = YAML_ANY_ENCODING;
#ifdef HAVE_RUBY_ENCODING_H
    int encoding = rb_utf8_encindex();
    rb_encoding * internal_enc = rb_default_internal_encoding();
#endif
    VALUE handler = rb_iv_get(self, "@handler");

    if (rb_scan_args(argc, argv, "11", &yaml, &path) == 1) {
	if(rb_respond_to(yaml, id_path))
	    path = rb_funcall(yaml, id_path, 0);
	else
	    path = rb_str_new2("<unknown>");
    }

    Data_Get_Struct(self, yaml_parser_t, parser);

    yaml_parser_delete(parser);
    yaml_parser_initialize(parser);

    if (OBJ_TAINTED(yaml)) tainted = 1;

    if (rb_respond_to(yaml, id_read)) {
#ifdef HAVE_RUBY_ENCODING_H
	yaml = transcode_io(yaml, &parser_encoding);
	yaml_parser_set_encoding(parser, parser_encoding);
#endif
	yaml_parser_set_input(parser, io_reader, (void *)yaml);
	if (RTEST(rb_obj_is_kind_of(yaml, rb_cIO))) tainted = 1;
    } else {
	StringValue(yaml);
#ifdef HAVE_RUBY_ENCODING_H
	yaml = transcode_string(yaml, &parser_encoding);
	yaml_parser_set_encoding(parser, parser_encoding);
#endif
	yaml_parser_set_input_string(
		parser,
		(const unsigned char *)RSTRING_PTR(yaml),
		(size_t)RSTRING_LEN(yaml)
		);
    }

    while(!done) {
	if(!yaml_parser_parse(parser, &event)) {
	    VALUE exception;

	    exception = make_exception(parser, path);
	    yaml_parser_delete(parser);
	    yaml_parser_initialize(parser);

	    rb_exc_raise(exception);
	}

	switch(event.type) {
	    case YAML_STREAM_START_EVENT:
	      {
		  VALUE args[2];

		  args[0] = handler;
		  args[1] = INT2NUM((long)event.data.stream_start.encoding);
		  rb_protect(protected_start_stream, (VALUE)args, &state);
	      }
	      break;
	  case YAML_DOCUMENT_START_EVENT:
	    {
		VALUE args[4];
		/* Get a list of tag directives (if any) */
		VALUE tag_directives = rb_ary_new();
		/* Grab the document version */
		VALUE version = event.data.document_start.version_directive ?
		    rb_ary_new3(
			(long)2,
			INT2NUM((long)event.data.document_start.version_directive->major),
			INT2NUM((long)event.data.document_start.version_directive->minor)
			) : rb_ary_new();

		if(event.data.document_start.tag_directives.start) {
		    yaml_tag_directive_t *start =
			event.data.document_start.tag_directives.start;
		    yaml_tag_directive_t *end =
			event.data.document_start.tag_directives.end;
		    for(; start != end; start++) {
			VALUE handle = Qnil;
			VALUE prefix = Qnil;
			if(start->handle) {
			    handle = rb_str_new2((const char *)start->handle);
			    if (tainted) OBJ_TAINT(handle);
#ifdef HAVE_RUBY_ENCODING_H
			    PSYCH_TRANSCODE(handle, encoding, internal_enc);
#endif
			}

			if(start->prefix) {
			    prefix = rb_str_new2((const char *)start->prefix);
			    if (tainted) OBJ_TAINT(prefix);
#ifdef HAVE_RUBY_ENCODING_H
			    PSYCH_TRANSCODE(prefix, encoding, internal_enc);
#endif
			}

			rb_ary_push(tag_directives, rb_ary_new3((long)2, handle, prefix));
		    }
		}
		args[0] = handler;
		args[1] = version;
		args[2] = tag_directives;
		args[3] = event.data.document_start.implicit == 1 ? Qtrue : Qfalse;
		rb_protect(protected_start_document, (VALUE)args, &state);
	    }
	    break;
	  case YAML_DOCUMENT_END_EVENT:
	    {
		VALUE args[2];

		args[0] = handler;
		args[1] = event.data.document_end.implicit == 1 ? Qtrue : Qfalse;
		rb_protect(protected_end_document, (VALUE)args, &state);
	    }
	    break;
	  case YAML_ALIAS_EVENT:
	    {
		VALUE args[2];
		VALUE alias = Qnil;
		if(event.data.alias.anchor) {
		    alias = rb_str_new2((const char *)event.data.alias.anchor);
		    if (tainted) OBJ_TAINT(alias);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(alias, encoding, internal_enc);
#endif
		}

		args[0] = handler;
		args[1] = alias;
		rb_protect(protected_alias, (VALUE)args, &state);
	    }
	    break;
	  case YAML_SCALAR_EVENT:
	    {
		VALUE args[7];
		VALUE anchor = Qnil;
		VALUE tag = Qnil;
		VALUE plain_implicit, quoted_implicit, style;
		VALUE val = rb_str_new(
		    (const char *)event.data.scalar.value,
		    (long)event.data.scalar.length
		    );
		if (tainted) OBJ_TAINT(val);

#ifdef HAVE_RUBY_ENCODING_H
		PSYCH_TRANSCODE(val, encoding, internal_enc);
#endif

		if(event.data.scalar.anchor) {
		    anchor = rb_str_new2((const char *)event.data.scalar.anchor);
		    if (tainted) OBJ_TAINT(anchor);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(anchor, encoding, internal_enc);
#endif
		}

		if(event.data.scalar.tag) {
		    tag = rb_str_new2((const char *)event.data.scalar.tag);
		    if (tainted) OBJ_TAINT(tag);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(tag, encoding, internal_enc);
#endif
		}

		plain_implicit =
		    event.data.scalar.plain_implicit == 0 ? Qfalse : Qtrue;

		quoted_implicit =
		    event.data.scalar.quoted_implicit == 0 ? Qfalse : Qtrue;

		style = INT2NUM((long)event.data.scalar.style);

		args[0] = handler;
		args[1] = val;
		args[2] = anchor;
		args[3] = tag;
		args[4] = plain_implicit;
		args[5] = quoted_implicit;
		args[6] = style;
		rb_protect(protected_scalar, (VALUE)args, &state);
	    }
	    break;
	  case YAML_SEQUENCE_START_EVENT:
	    {
		VALUE args[5];
		VALUE anchor = Qnil;
		VALUE tag = Qnil;
		VALUE implicit, style;
		if(event.data.sequence_start.anchor) {
		    anchor = rb_str_new2((const char *)event.data.sequence_start.anchor);
		    if (tainted) OBJ_TAINT(anchor);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(anchor, encoding, internal_enc);
#endif
		}

		tag = Qnil;
		if(event.data.sequence_start.tag) {
		    tag = rb_str_new2((const char *)event.data.sequence_start.tag);
		    if (tainted) OBJ_TAINT(tag);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(tag, encoding, internal_enc);
#endif
		}

		implicit =
		    event.data.sequence_start.implicit == 0 ? Qfalse : Qtrue;

		style = INT2NUM((long)event.data.sequence_start.style);

		args[0] = handler;
		args[1] = anchor;
		args[2] = tag;
		args[3] = implicit;
		args[4] = style;

		rb_protect(protected_start_sequence, (VALUE)args, &state);
	    }
	    break;
	  case YAML_SEQUENCE_END_EVENT:
	    rb_protect(protected_end_sequence, handler, &state);
	    break;
	  case YAML_MAPPING_START_EVENT:
	    {
		VALUE args[5];
		VALUE anchor = Qnil;
		VALUE tag = Qnil;
		VALUE implicit, style;
		if(event.data.mapping_start.anchor) {
		    anchor = rb_str_new2((const char *)event.data.mapping_start.anchor);
		    if (tainted) OBJ_TAINT(anchor);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(anchor, encoding, internal_enc);
#endif
		}

		if(event.data.mapping_start.tag) {
		    tag = rb_str_new2((const char *)event.data.mapping_start.tag);
		    if (tainted) OBJ_TAINT(tag);
#ifdef HAVE_RUBY_ENCODING_H
		    PSYCH_TRANSCODE(tag, encoding, internal_enc);
#endif
		}

		implicit =
		    event.data.mapping_start.implicit == 0 ? Qfalse : Qtrue;

		style = INT2NUM((long)event.data.mapping_start.style);

		args[0] = handler;
		args[1] = anchor;
		args[2] = tag;
		args[3] = implicit;
		args[4] = style;

		rb_protect(protected_start_mapping, (VALUE)args, &state);
	    }
	    break;
	  case YAML_MAPPING_END_EVENT:
	    rb_protect(protected_end_mapping, handler, &state);
	    break;
	  case YAML_NO_EVENT:
	    rb_protect(protected_empty, handler, &state);
	    break;
	  case YAML_STREAM_END_EVENT:
	    rb_protect(protected_end_stream, handler, &state);
	    done = 1;
	    break;
	}
	yaml_event_delete(&event);
	if (state) rb_jump_tag(state);
    }

    return self;
}

/*
 * call-seq:
 *    parser.mark # => #<Psych::Parser::Mark>
 *
 * Returns a Psych::Parser::Mark object that contains line, column, and index
 * information.
 */
static VALUE mark(VALUE self)
{
    VALUE mark_klass;
    VALUE args[3];
    yaml_parser_t * parser;

    Data_Get_Struct(self, yaml_parser_t, parser);
    mark_klass = rb_const_get_at(cPsychParser, rb_intern("Mark"));
    args[0] = INT2NUM(parser->mark.index);
    args[1] = INT2NUM(parser->mark.line);
    args[2] = INT2NUM(parser->mark.column);

    return rb_class_new_instance(3, args, mark_klass);
}

void Init_psych_parser()
{
#if 0
    mPsych = rb_define_module("Psych");
#endif

    cPsychParser = rb_define_class_under(mPsych, "Parser", rb_cObject);
    rb_define_alloc_func(cPsychParser, allocate);

    /* Any encoding: Let the parser choose the encoding */
    rb_define_const(cPsychParser, "ANY", INT2NUM(YAML_ANY_ENCODING));

    /* UTF-8 Encoding */
    rb_define_const(cPsychParser, "UTF8", INT2NUM(YAML_UTF8_ENCODING));

    /* UTF-16-LE Encoding with BOM */
    rb_define_const(cPsychParser, "UTF16LE", INT2NUM(YAML_UTF16LE_ENCODING));

    /* UTF-16-BE Encoding with BOM */
    rb_define_const(cPsychParser, "UTF16BE", INT2NUM(YAML_UTF16BE_ENCODING));

    rb_require("psych/syntax_error");
    ePsychSyntaxError = rb_define_class_under(mPsych, "SyntaxError", rb_eSyntaxError);

    rb_define_method(cPsychParser, "parse", parse, -1);
    rb_define_method(cPsychParser, "mark", mark, 0);

    id_read           = rb_intern("read");
    id_path           = rb_intern("path");
    id_empty          = rb_intern("empty");
    id_start_stream   = rb_intern("start_stream");
    id_end_stream     = rb_intern("end_stream");
    id_start_document = rb_intern("start_document");
    id_end_document   = rb_intern("end_document");
    id_alias          = rb_intern("alias");
    id_scalar         = rb_intern("scalar");
    id_start_sequence = rb_intern("start_sequence");
    id_end_sequence   = rb_intern("end_sequence");
    id_start_mapping  = rb_intern("start_mapping");
    id_end_mapping    = rb_intern("end_mapping");
}
/* vim: set noet sws=4 sw=4: */
