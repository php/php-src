--TEST--
JIT INIT_FCALL: 002 incorrect megamorphic call detection
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_max_polymorphic_calls=0
--FILE--
<?php
class C {
	function foo($x) {
		return $x;
	}
}
function foo($x) {
	return $x;
}
function test2($x) {
	return foo(foo($x));
}
function test1() {
	$x = new C;
	foo(foo($x->foo(foo(test2($x)))));
}
test1();
?>
DONE
--EXPECT--
DONE
