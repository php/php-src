--TEST--
GH-10635: Function JIT causes impossible assertion
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
trait T {
	function foo() {
		return reset($this->a);
	}
}
class C {
	use T;
	private array $a = [1];
}
$o = new C;
$o->foo();
unset($o);
$o = new C;
$o->foo();
unset($o);
?>
DONE
--EXPECT--
DONE
