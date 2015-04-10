--TEST--
ZE2 type hinting
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
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

Fatal error: Argument 1 passed to type_hint_foo() must be an instance of Foo, instance of Bar given, called in %s on line 16 and defined in %s on line 9
