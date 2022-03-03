--TEST--
JIT SEND_REF: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
function foo(&$obj) {
}
class A {
	function foo() {
		for ($i = 0; $i < 10; $i++) {
			foo($this);
		}
		echo "ok\n";
	}
}
$a = new A;
$a->foo();
?>
--EXPECT--
ok
