<?php
function f1() {
}

if (isset($rt)) {
	function f2() {
	}
}

interface a {
	function foo();
	function bar();
}
interface b {
	function foo();
}

abstract class c {
	function bar() { }
}

class x extends c implements a, b {
	function foo() { }
}

trait T1 {
	static function foo() {
		var_dump(__METHOD__);
	}
}
trait T2 {
	use T1;
	static function bar() {
		var_dump(__METHOD__);
	}
}
class Y {
	use T2;
}

class Z {
    public $foo;
    public a $bar;
}

class Z2 extends Z {}

function get_anon() {
    return new class {};
}

if (!isset($rt)) {
	eval("class Foo {}");

	class Bar extends Foo {}

	eval("function f3() {} ");
}
