--TEST--
JIT RET: 004 Return a reference when it's not expected
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=32M
--FILE--
<?php
class A {
	function foo() {
	}
	function bar() {
		$x = $this->foo();
		var_dump(str_repeat($x,5));
	}
}
class B extends A {
	public $prop = "x";
	function &foo() {
		return $this->prop;
	}
}
$b = new B;
$b->bar();
?>
--EXPECT--
string(5) "xxxxx"
