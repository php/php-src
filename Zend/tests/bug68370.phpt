--TEST--
Bug #68370 "unset($this)" can make the program crash
--FILE--
<?php
class C {
	public function test() {
		unset($this);
		return get_defined_vars();
	}
}
$c = new C();
$x = $c->test();
print_r($x);
unset($c, $x);
--EXPECTF--
Fatal error: Cannot unset $this in %sbug68370.php on line 4
