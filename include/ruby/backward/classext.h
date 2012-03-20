#if   defined __GNUC__
#warning use of RClass internals is deprecated
#elif defined _MSC_VER || defined __BORLANDC__
#pragma message("warning: use of RClass internals is deprecated")
#endif

#ifndef RUBY_BACKWARD_CLASSEXT_H
#define RUBY_BACKWARD_CLASSEXT_H 1

typedef struct rb_deprecated_classext_struct {
    VALUE super;
} rb_deprecated_classext_t;

#undef RCLASS_SUPER(c)
#define RCLASS_EXT(c) ((rb_deprecated_classext_t *)RCLASS(c)->ptr)
#define RCLASS_SUPER(c) (RCLASS_EXT(c)->super)

#endif	/* RUBY_BACKWARD_CLASSEXT_H */
