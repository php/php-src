--TEST--
JIT METHOD_CALL: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--EXTENSIONS--
opcache
--FILE--
<?php
function &foo() {
	return A::$o;
}
class A {
	static $o = null;
	static function foo() {
		return foo()->bar();
	}
	static function loop() {
		for ($i = 0; $i < 10; $i++) {
			self::foo();
		}
		echo "ok\n";
	}
}
class B {
	function bar() {
	}
}
A::$o = new B;
A::loop();
?>
--EXPECT--
ok
