#include <psych.h>

VALUE cPsychVisitorsToRuby;

/* call-seq: vis.build_exception(klass, message)
 *
 * Create an exception with class +klass+ and +message+
 */
static VALUE build_exception(VALUE self, VALUE klass, VALUE mesg)
{
    VALUE e = rb_obj_alloc(klass);

    rb_iv_set(e, "mesg", mesg);

    return e;
}

/* call-seq: vis.path2class(path)
 *
 * Convert +path+ string to a class
 */
static VALUE path2class(VALUE self, VALUE path)
{
#ifdef HAVE_RUBY_ENCODING_H
    return rb_path_to_class(path);
#else
    return rb_path2class(StringValuePtr(path));
#endif
}

void Init_psych_to_ruby(void)
{
    VALUE psych     = rb_define_module("Psych");
    VALUE visitors  = rb_define_module_under(psych, "Visitors");
    VALUE visitor   = rb_define_class_under(visitors, "Visitor", rb_cObject);
    cPsychVisitorsToRuby = rb_define_class_under(visitors, "ToRuby", visitor);

    rb_define_private_method(cPsychVisitorsToRuby, "build_exception", build_exception, 2);
    rb_define_private_method(cPsychVisitorsToRuby, "path2class", path2class, 1);
}
/* vim: set noet sws=4 sw=4: */
