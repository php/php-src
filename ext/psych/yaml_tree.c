#include <psych.h>

VALUE cPsychVisitorsYamlTree;

/*
 * call-seq: private_iv_get(target, prop)
 *
 * Get the private instance variable +prop+ from +target+
 */
static VALUE private_iv_get(VALUE self, VALUE target, VALUE prop)
{
    return rb_attr_get(target, rb_intern(StringValuePtr(prop)));
}

void Init_psych_yaml_tree(void)
{
    VALUE psych     = rb_define_module("Psych");
    VALUE visitors  = rb_define_module_under(psych, "Visitors");
    VALUE visitor   = rb_define_class_under(visitors, "Visitor", rb_cObject);
    cPsychVisitorsYamlTree = rb_define_class_under(visitors, "YAMLTree", visitor);

    rb_define_private_method(cPsychVisitorsYamlTree, "private_iv_get", private_iv_get, 2);
}
/* vim: set noet sws=4 sw=4: */
