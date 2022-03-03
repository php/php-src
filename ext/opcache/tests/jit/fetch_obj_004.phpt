--TEST--
JIT: FETCH_OBJ 004
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
#[AllowDynamicProperties]
class C {
    var $a = 0;
}
function foo() {
    $x = new C;
    $x->a = 1;
    unset($x->a);
    $x->a = 3;
    var_dump($x);
}
function bar() {
    $x = new C;
    $x->a = 1;
    $x->b = 2;
    unset($x->a);
    $x->a = 3;
    var_dump($x);
}
foo();
bar();
?>
--EXPECT--
object(C)#1 (1) {
  ["a"]=>
  int(3)
}
object(C)#1 (2) {
  ["a"]=>
  int(3)
  ["b"]=>
  int(2)
}
