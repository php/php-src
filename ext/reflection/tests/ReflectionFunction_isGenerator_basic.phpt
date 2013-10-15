--TEST--
ReflectionFunction::isGenerator()
--FILE--
<?php

$closure1 = function() {return "this is a closure"; };
$closure2 = function($param) {
	yield $param;
};

$rf1 = new ReflectionFunction($closure1);
var_dump($rf1->isGenerator());

$rf2 = new ReflectionFunction($closure2);
var_dump($rf2->isGenerator());

function func1() {
	return 'func1';
}

function func2() {
	yield 'func2';
}

$rf1 = new ReflectionFunction('func1');
var_dump($rf1->isGenerator());

$rf2 = new ReflectionFunction('func2');
var_dump($rf2->isGenerator());


class Foo {
	public function f1() {
	}

	public function f2() {
		yield;
	}
}

$rc = new ReflectionClass('Foo');
foreach($rc->getMethods() as $m) {
	var_dump($m->isGenerator());
}
?>
--EXPECTF--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
