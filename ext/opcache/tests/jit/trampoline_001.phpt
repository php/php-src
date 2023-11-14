--TEST--
JIT Trampoline 001: trampoline cleanup
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
--EXTENSIONS--
opcache
--FILE--
<?php
class A {
}
class B extends A {
	function foo() {
		echo "B";
	}
}
class C extends A {
	function __call($name, $argd) {
		echo "C";
	}
}
$b = new B;
$c = new C;
$a = [$b, $b, $b, $c, $c, $c];
foreach ($a as $x) {
	$x->foo();
}
echo "\n";
?>
--EXPECT--
BBBCCC
