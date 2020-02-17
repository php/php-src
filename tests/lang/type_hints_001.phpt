--TEST--
ZE2 type hinting
--FILE--
<?php

class Foo {
}

class Bar {
}

function type_hint_foo(Foo $a) {
}

$foo = new Foo;
$bar = new Bar;

type_hint_foo($foo);
type_hint_foo($bar);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: type_hint_foo() expects argument #1 ($a) to be of type Foo, Bar given, called in %s on line 16 and defined in %s:9
Stack trace:
#0 %s(%d): type_hint_foo(Object(Bar))
#1 {main}
  thrown in %s on line 9
