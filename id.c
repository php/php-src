/**********************************************************************

  id.c -

  $Author$
  created at: Thu Jul 12 04:37:51 2007

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#include "ruby/ruby.h"

#include "id.h"

static void
Init_id(void)
{
#undef rb_intern
#define rb_intern(str) rb_intern_const(str)
    rb_encoding *enc = rb_usascii_encoding();

    REGISTER_SYMID(idNULL, "");
    REGISTER_SYMID(idIFUNC, "<IFUNC>");
    REGISTER_SYMID(idCFUNC, "<CFUNC>");
    REGISTER_SYMID(idRespond_to, "respond_to?");

    REGISTER_SYMID(id_core_set_method_alias,   	    "core#set_method_alias");
    REGISTER_SYMID(id_core_set_variable_alias, 	    "core#set_variable_alias");
    REGISTER_SYMID(id_core_undef_method,       	    "core#undef_method");
    REGISTER_SYMID(id_core_define_method,      	    "core#define_method");
    REGISTER_SYMID(id_core_define_singleton_method, "core#define_singleton_method");
    REGISTER_SYMID(id_core_set_postexe,             "core#set_postexe");

    REGISTER_SYMID(idEach, "each");
    REGISTER_SYMID(idLength, "length");
    REGISTER_SYMID(idSize, "size");
    REGISTER_SYMID(idLambda, "lambda");
    REGISTER_SYMID(idIntern, "intern");
    REGISTER_SYMID(idGets, "gets");
    REGISTER_SYMID(idSucc, "succ");
    REGISTER_SYMID(idMethodMissing, "method_missing");
#if SUPPORT_JOKE
    REGISTER_SYMID(idBitblt, "bitblt");
    REGISTER_SYMID(idAnswer, "the_answer_to_life_the_universe_and_everything");
#endif
    REGISTER_SYMID(idSend, "send");
    REGISTER_SYMID(id__send__, "__send__");
    REGISTER_SYMID(idInitialize, "initialize");
    REGISTER_SYMID(idUScore, "_");
}
