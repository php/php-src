#include <psych.h>

VALUE cPsychEmitter;
static ID id_write;

static void emit(yaml_emitter_t * emitter, yaml_event_t * event)
{
    if(!yaml_emitter_emit(emitter, event))
	rb_raise(rb_eRuntimeError, "%s", emitter->problem);
}

static int writer(void *ctx, unsigned char *buffer, size_t size)
{
    VALUE io = (VALUE)ctx;
    VALUE str = rb_str_new((const char *)buffer, (long)size);
    VALUE wrote = rb_funcall(io, id_write, 1, str);
    return (int)NUM2INT(wrote);
}

static void dealloc(void * ptr)
{
    yaml_emitter_t * emitter;

    emitter = (yaml_emitter_t *)ptr;
    yaml_emitter_delete(emitter);
    xfree(emitter);
}

static VALUE allocate(VALUE klass)
{
    yaml_emitter_t * emitter;

    emitter = xmalloc(sizeof(yaml_emitter_t));

    yaml_emitter_initialize(emitter);
    yaml_emitter_set_unicode(emitter, 1);
    yaml_emitter_set_indent(emitter, 2);

    return Data_Wrap_Struct(klass, 0, dealloc, emitter);
}

/* call-seq: Psych::Emitter.new(io)
 *
 * Create a new Psych::Emitter that writes to +io+.
 */
static VALUE initialize(VALUE self, VALUE io)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_emitter_set_output(emitter, writer, (void *)io);

    return self;
}

/* call-seq: emitter.start_stream(encoding)
 *
 * Start a stream emission with +encoding+
 *
 * See Psych::Handler#start_stream
 */
static VALUE start_stream(VALUE self, VALUE encoding)
{
    yaml_emitter_t * emitter;
    yaml_event_t event;
    Data_Get_Struct(self, yaml_emitter_t, emitter);
    Check_Type(encoding, T_FIXNUM);

    yaml_stream_start_event_initialize(&event, (yaml_encoding_t)NUM2INT(encoding));

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.end_stream
 *
 * End a stream emission
 *
 * See Psych::Handler#end_stream
 */
static VALUE end_stream(VALUE self)
{
    yaml_emitter_t * emitter;
    yaml_event_t event;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_stream_end_event_initialize(&event);

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.start_document(version, tags, implicit)
 *
 * Start a document emission with YAML +version+, +tags+, and an +implicit+
 * start.
 *
 * See Psych::Handler#start_document
 */
static VALUE start_document(VALUE self, VALUE version, VALUE tags, VALUE imp)
{
    yaml_emitter_t * emitter;
    yaml_tag_directive_t * head = NULL;
    yaml_tag_directive_t * tail = NULL;
    yaml_event_t event;
    yaml_version_directive_t version_directive;
    Data_Get_Struct(self, yaml_emitter_t, emitter);


    Check_Type(version, T_ARRAY);

    if(RARRAY_LEN(version) > 0) {
	VALUE major = rb_ary_entry(version, (long)0);
	VALUE minor = rb_ary_entry(version, (long)1);

	version_directive.major = NUM2INT(major);
	version_directive.minor = NUM2INT(minor);
    }

    if(RTEST(tags)) {
	int i = 0;
#ifdef HAVE_RUBY_ENCODING_H
	rb_encoding * encoding = rb_utf8_encoding();
#endif

	Check_Type(tags, T_ARRAY);

	head  = xcalloc((size_t)RARRAY_LEN(tags), sizeof(yaml_tag_directive_t));
	tail  = head;

	for(i = 0; i < RARRAY_LEN(tags); i++) {
	    VALUE tuple = RARRAY_PTR(tags)[i];
	    VALUE name;
	    VALUE value;

	    Check_Type(tuple, T_ARRAY);

	    if(RARRAY_LEN(tuple) < 2) {
		xfree(head);
		rb_raise(rb_eRuntimeError, "tag tuple must be of length 2");
	    }
	    name  = RARRAY_PTR(tuple)[0];
	    value = RARRAY_PTR(tuple)[1];
#ifdef HAVE_RUBY_ENCODING_H
	    name = rb_str_export_to_enc(name, encoding);
	    value = rb_str_export_to_enc(value, encoding);
#endif

	    tail->handle = (yaml_char_t *)StringValuePtr(name);
	    tail->prefix = (yaml_char_t *)StringValuePtr(value);

	    tail++;
	}
    }

    yaml_document_start_event_initialize(
	    &event,
	    (RARRAY_LEN(version) > 0) ? &version_directive : NULL,
	    head,
	    tail,
	    imp ? 1 : 0
	    );

    emit(emitter, &event);

    if(head) xfree(head);

    return self;
}

/* call-seq: emitter.end_document(implicit)
 *
 * End a document emission with an +implicit+ ending.
 *
 * See Psych::Handler#end_document
 */
static VALUE end_document(VALUE self, VALUE imp)
{
    yaml_emitter_t * emitter;
    yaml_event_t event;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_document_end_event_initialize(&event, imp ? 1 : 0);

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.scalar(value, anchor, tag, plain, quoted, style)
 *
 * Emit a scalar with +value+, +anchor+, +tag+, and a +plain+ or +quoted+
 * string type with +style+.
 *
 * See Psych::Handler#scalar
 */
static VALUE scalar(
	VALUE self,
	VALUE value,
	VALUE anchor,
	VALUE tag,
	VALUE plain,
	VALUE quoted,
	VALUE style
	) {
    yaml_emitter_t * emitter;
    yaml_event_t event;
#ifdef HAVE_RUBY_ENCODING_H
    rb_encoding *encoding;
#endif
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    Check_Type(value, T_STRING);

#ifdef HAVE_RUBY_ENCODING_H
    encoding = rb_utf8_encoding();

    value = rb_str_export_to_enc(value, encoding);

    if(!NIL_P(anchor)) {
	Check_Type(anchor, T_STRING);
	anchor = rb_str_export_to_enc(anchor, encoding);
    }

    if(!NIL_P(tag)) {
	Check_Type(tag, T_STRING);
	tag = rb_str_export_to_enc(tag, encoding);
    }
#endif

    yaml_scalar_event_initialize(
	    &event,
	    (yaml_char_t *)(NIL_P(anchor) ? NULL : StringValuePtr(anchor)),
	    (yaml_char_t *)(NIL_P(tag) ? NULL : StringValuePtr(tag)),
	    (yaml_char_t*)StringValuePtr(value),
	    (int)RSTRING_LEN(value),
	    plain ? 1 : 0,
	    quoted ? 1 : 0,
	    (yaml_scalar_style_t)NUM2INT(style)
	    );

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.start_sequence(anchor, tag, implicit, style)
 *
 * Start emitting a sequence with +anchor+, a +tag+, +implicit+ sequence
 * start and end, along with +style+.
 *
 * See Psych::Handler#start_sequence
 */
static VALUE start_sequence(
	VALUE self,
	VALUE anchor,
	VALUE tag,
	VALUE implicit,
	VALUE style
	) {
    yaml_emitter_t * emitter;
    yaml_event_t event;

#ifdef HAVE_RUBY_ENCODING_H
    rb_encoding * encoding = rb_utf8_encoding();

    if(!NIL_P(anchor)) {
	Check_Type(anchor, T_STRING);
	anchor = rb_str_export_to_enc(anchor, encoding);
    }

    if(!NIL_P(tag)) {
	Check_Type(tag, T_STRING);
	tag = rb_str_export_to_enc(tag, encoding);
    }
#endif

    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_sequence_start_event_initialize(
	    &event,
	    (yaml_char_t *)(NIL_P(anchor) ? NULL : StringValuePtr(anchor)),
	    (yaml_char_t *)(NIL_P(tag) ? NULL : StringValuePtr(tag)),
	    implicit ? 1 : 0,
	    (yaml_sequence_style_t)NUM2INT(style)
	    );

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.end_sequence
 *
 * End sequence emission.
 *
 * See Psych::Handler#end_sequence
 */
static VALUE end_sequence(VALUE self)
{
    yaml_emitter_t * emitter;
    yaml_event_t event;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_sequence_end_event_initialize(&event);

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.start_mapping(anchor, tag, implicit, style)
 *
 * Start emitting a YAML map with +anchor+, +tag+, an +implicit+ start
 * and end, and +style+.
 *
 * See Psych::Handler#start_mapping
 */
static VALUE start_mapping(
	VALUE self,
	VALUE anchor,
	VALUE tag,
	VALUE implicit,
	VALUE style
	) {
    yaml_emitter_t * emitter;
    yaml_event_t event;
#ifdef HAVE_RUBY_ENCODING_H
    rb_encoding *encoding;
#endif
    Data_Get_Struct(self, yaml_emitter_t, emitter);

#ifdef HAVE_RUBY_ENCODING_H
    encoding = rb_utf8_encoding();

    if(!NIL_P(anchor)) {
	Check_Type(anchor, T_STRING);
	anchor = rb_str_export_to_enc(anchor, encoding);
    }

    if(!NIL_P(tag)) {
	Check_Type(tag, T_STRING);
	tag = rb_str_export_to_enc(tag, encoding);
    }
#endif

    yaml_mapping_start_event_initialize(
	    &event,
	    (yaml_char_t *)(NIL_P(anchor) ? NULL : StringValuePtr(anchor)),
	    (yaml_char_t *)(NIL_P(tag) ? NULL : StringValuePtr(tag)),
	    implicit ? 1 : 0,
	    (yaml_mapping_style_t)NUM2INT(style)
	    );

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.end_mapping
 *
 * Emit the end of a mapping.
 *
 * See Psych::Handler#end_mapping
 */
static VALUE end_mapping(VALUE self)
{
    yaml_emitter_t * emitter;
    yaml_event_t event;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_mapping_end_event_initialize(&event);

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.alias(anchor)
 *
 * Emit an alias with +anchor+.
 *
 * See Psych::Handler#alias
 */
static VALUE alias(VALUE self, VALUE anchor)
{
    yaml_emitter_t * emitter;
    yaml_event_t event;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

#ifdef HAVE_RUBY_ENCODING_H
    if(!NIL_P(anchor)) {
	Check_Type(anchor, T_STRING);
	anchor = rb_str_export_to_enc(anchor, rb_utf8_encoding());
    }
#endif

    yaml_alias_event_initialize(
	    &event,
	    (yaml_char_t *)(NIL_P(anchor) ? NULL : StringValuePtr(anchor))
	    );

    emit(emitter, &event);

    return self;
}

/* call-seq: emitter.canonical = true
 *
 * Set the output style to canonical, or not.
 */
static VALUE set_canonical(VALUE self, VALUE style)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_emitter_set_canonical(emitter, Qtrue == style ? 1 : 0);

    return style;
}

/* call-seq: emitter.canonical
 *
 * Get the output style, canonical or not.
 */
static VALUE canonical(VALUE self)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    return (emitter->canonical == 0) ? Qfalse : Qtrue;
}

/* call-seq: emitter.indentation = level
 *
 * Set the indentation level to +level+.  The level must be less than 10 and
 * greater than 1.
 */
static VALUE set_indentation(VALUE self, VALUE level)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_emitter_set_indent(emitter, NUM2INT(level));

    return level;
}

/* call-seq: emitter.indentation
 *
 * Get the indentation level.
 */
static VALUE indentation(VALUE self)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    return INT2NUM(emitter->best_indent);
}

/* call-seq: emitter.line_width
 *
 * Get the preferred line width.
 */
static VALUE line_width(VALUE self)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    return INT2NUM(emitter->best_width);
}

/* call-seq: emitter.line_width = width
 *
 * Set the preferred line with to +width+.
 */
static VALUE set_line_width(VALUE self, VALUE width)
{
    yaml_emitter_t * emitter;
    Data_Get_Struct(self, yaml_emitter_t, emitter);

    yaml_emitter_set_width(emitter, NUM2INT(width));

    return width;
}

void Init_psych_emitter()
{
    VALUE psych     = rb_define_module("Psych");
    VALUE handler   = rb_define_class_under(psych, "Handler", rb_cObject);
    cPsychEmitter   = rb_define_class_under(psych, "Emitter", handler);

    rb_define_alloc_func(cPsychEmitter, allocate);

    rb_define_method(cPsychEmitter, "initialize", initialize, 1);
    rb_define_method(cPsychEmitter, "start_stream", start_stream, 1);
    rb_define_method(cPsychEmitter, "end_stream", end_stream, 0);
    rb_define_method(cPsychEmitter, "start_document", start_document, 3);
    rb_define_method(cPsychEmitter, "end_document", end_document, 1);
    rb_define_method(cPsychEmitter, "scalar", scalar, 6);
    rb_define_method(cPsychEmitter, "start_sequence", start_sequence, 4);
    rb_define_method(cPsychEmitter, "end_sequence", end_sequence, 0);
    rb_define_method(cPsychEmitter, "start_mapping", start_mapping, 4);
    rb_define_method(cPsychEmitter, "end_mapping", end_mapping, 0);
    rb_define_method(cPsychEmitter, "alias", alias, 1);
    rb_define_method(cPsychEmitter, "canonical", canonical, 0);
    rb_define_method(cPsychEmitter, "canonical=", set_canonical, 1);
    rb_define_method(cPsychEmitter, "indentation", indentation, 0);
    rb_define_method(cPsychEmitter, "indentation=", set_indentation, 1);
    rb_define_method(cPsychEmitter, "line_width", line_width, 0);
    rb_define_method(cPsychEmitter, "line_width=", set_line_width, 1);

    id_write = rb_intern("write");
}
/* vim: set noet sws=4 sw=4: */
