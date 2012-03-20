/**********************************************************************

  inits.c -

  $Author$
  created at: Tue Dec 28 16:01:58 JST 1993

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#include "ruby/ruby.h"
#include "internal.h"

#define CALL(n) {void Init_##n(void); Init_##n();}

void
rb_call_inits(void)
{
    CALL(RandomSeed);
    CALL(sym);
    CALL(var_tables);
    CALL(Object);
    CALL(top_self);
    CALL(Encoding);
    CALL(Comparable);
    CALL(Enumerable);
    CALL(String);
    CALL(Exception);
    CALL(eval);
    CALL(safe);
    CALL(jump);
    CALL(Numeric);
    CALL(Bignum);
    CALL(syserr);
    CALL(Array);
    CALL(Hash);
    CALL(Struct);
    CALL(Regexp);
    CALL(pack);
    CALL(transcode);
    CALL(marshal);
    CALL(Range);
    CALL(IO);
    CALL(Dir);
    CALL(Time);
    CALL(Random);
    CALL(signal);
    CALL(process);
    CALL(load);
    CALL(Proc);
    CALL(Binding);
    CALL(Math);
    CALL(GC);
    CALL(Enumerator);
    CALL(VM);
    CALL(ISeq);
    CALL(Thread);
    CALL(Cont);
    CALL(Rational);
    CALL(Complex);
    CALL(version);
}
#undef CALL
