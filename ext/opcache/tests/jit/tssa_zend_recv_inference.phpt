--TEST--
TSSA ZEND_RECV inference
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_debug=2
--FILE--
<?php
final class Foo {
  public $prop = 0;
}
function test(Foo $obj) {
  $obj->prop=1;
}

$foo = new Foo;
for ($i=0;$i<3;$i++) {
  test($foo);
}
?>
--EXPECTF--
%A
test:
     ; (lines=4, args=1, vars=1, tmps=%d, ssa_vars=3)
     ; (JIT)
     ; %s
     ; return  [null] RANGE[0..0]
     ; #0.CV0($obj) NOVAL [undef]
BB0:
     ; start lines=[0-0]
     ; to=(BB1)
     ; level=0
     ; children=(BB1)
0000 #1.CV0($obj) [rc1, rcn, object (Foo)] = RECV 1

BB1:
     ; recv follow exit entry lines=[1-3]
     ; from=(BB0)
     ; idom=BB0
     ; level=1
0001 ASSIGN_OBJ #1.CV0($obj) [rc1, rcn, object (Foo)] -> #2.CV0($obj) NOVAL [rc1, rcn, object (Foo)] string("prop")
0002 OP_DATA int(1)
0003 RETURN null
%A
