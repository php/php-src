--TEST--
Bug #27468 (foreach in __destruct() causes segfault)
--FILE--
<?php
class foo {
	function __destruct() {
		foreach ($this->x as $x);
	}
}
new foo();
echo 'OK';
?>
--EXPECTF--
Warning: Invalid argument supplied for foreach() in %sbug27468.php on line 4

Call Stack:
    0.0003      43704   1. {main}() %sbug27468.php:0
    0.0004      43768   2. foo::__destruct() %sbug27468.php:7
OK
