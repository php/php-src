--TEST--
Bug #34712 (zend.ze1_compatibility_mode = on segfault)
--INI--
zend.ze1_compatibility_mode=1
error_reporting=4095
--FILE--
<?php
class foo {
	function foo(&$obj_ref) {
		$this->bar = &$obj_ref;
	}
}		


class bar {
	function bar() {
		$this->foo = new foo($this);
	}
}

$test = new bar;
echo "ok\n";
?>
--EXPECTF--
Strict Standards: Implicit cloning object of class 'foo' because of 'zend.ze1_compatibility_mode' in %sbug34712.php on line 11

Strict Standards: Implicit cloning object of class 'bar' because of 'zend.ze1_compatibility_mode' in %sbug34712.php on line 15
ok
