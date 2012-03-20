#include <psych.h>

/* call-seq: Psych.libyaml_version
 *
 * Returns the version of libyaml being used
 */
static VALUE libyaml_version(VALUE module)
{
    int major, minor, patch;
    VALUE list[3];

    yaml_get_version(&major, &minor, &patch);

    list[0] = INT2NUM((long)major);
    list[1] = INT2NUM((long)minor);
    list[2] = INT2NUM((long)patch);

    return rb_ary_new4((long)3, list);
}

VALUE mPsych;

void Init_psych()
{
    mPsych = rb_define_module("Psych");

    rb_define_singleton_method(mPsych, "libyaml_version", libyaml_version, 0);

    Init_psych_parser();
    Init_psych_emitter();
    Init_psych_to_ruby();
    Init_psych_yaml_tree();
}
/* vim: set noet sws=4 sw=4: */
