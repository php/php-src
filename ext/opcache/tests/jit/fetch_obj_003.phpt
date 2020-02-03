--TEST--
JIT: FETCH_OBJ 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class C {
    var $a = 0;
}
function foo() {
    $x = new C;
    $x->a = 1;
    unset($x->a);
    $x->a += 2;
    var_dump($x);
}
function bar() {
    $x = new C;
    $x->a = 1;
    $x->b = 2;
    unset($x->a);
    $x->a += 2;
    var_dump($x);
}
foo();
bar();
?>
--EXPECTF--
Warning: Undefined property: C::$a in %s on line %d
object(C)#1 (1) {
  ["a"]=>
  int(2)
}

Warning: Undefined property: C::$a in %s on line %d
object(C)#1 (2) {
  ["a"]=>
  int(2)
  ["b"]=>
  int(2)
}
