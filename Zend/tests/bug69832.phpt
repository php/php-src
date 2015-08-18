--TEST--
Bug #69832 (Assertion failed in zend_compile_const_expr_magic_const)
--FILE--
<?php

class Test {
	public $foo = [Bar::A, __CLASS__][__CLASS__ != ""];
	public $bar = Bar::A && __CLASS__;
	public $baz = Bar::A ?: __CLASS__;
	public $buzz = Bar::A ? __CLASS__ : 0;
}

eval(<<<'PHP'
class Bar {
	const A = 1;
}
PHP
);

$t = new Test;
var_dump($t->foo);
var_dump($t->bar);
var_dump($t->baz);
var_dump($t->buzz);

?>
--EXPECT--
string(4) "Test"
bool(true)
int(1)
string(4) "Test"

