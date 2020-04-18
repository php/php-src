--TEST--
JIT: FETCH_OBJ 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class A {
   public $x = 2;
}

class B {
   public $x = 3;
   public function __get($name) {
      var_dump("__get");
   }
}

function bar() {
   $a = new A();
   var_dump($a->x);
   var_dump($a->y);
   $b = new B();
   var_dump($b->x);
   unset($b->x);
   $b->x;
}

bar();
?>
--EXPECTF--
int(2)

Warning: Undefined property: A::$y in %s on line %d
NULL
int(3)
string(5) "__get"
