/**********************************************************************

  constant.h -

  $Author$
  created at: Sun Nov 15 00:09:33 2009

  Copyright (C) 2009 Yusuke Endoh

**********************************************************************/
#ifndef CONSTANT_H
#define CONSTANT_H

typedef enum {
    CONST_PUBLIC    = 0x00,
    CONST_PRIVATE   = 0x01
} rb_const_flag_t;

typedef struct rb_const_entry_struct {
    rb_const_flag_t flag;
    VALUE value;            /* should be mark */
    VALUE file;
    int line;
} rb_const_entry_t;

VALUE rb_mod_private_constant(int argc, VALUE *argv, VALUE obj);
VALUE rb_mod_public_constant(int argc, VALUE *argv, VALUE obj);
void rb_free_const_table(st_table *tbl);
VALUE rb_public_const_get(VALUE klass, ID id);
VALUE rb_public_const_get_at(VALUE klass, ID id);
VALUE rb_public_const_get_from(VALUE klass, ID id);
int rb_public_const_defined(VALUE klass, ID id);
int rb_public_const_defined_at(VALUE klass, ID id);
int rb_public_const_defined_from(VALUE klass, ID id);

#endif /* CONSTANT_H */
