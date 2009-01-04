--TEST--
Closure 036: Rebinding closure $this on property access, using scope
--FILE--
<?php

$instance = 0;

class Test {
	private $value = 42;
	function __construct() {
		global $instance;
		$this->instance = ++$instance;
	}
}

$o = new Test;
$o->func = function () {
	var_dump($this->value);
};
$func = $o->func;
$func();

var_dump($instance);
?>
===DONE===
--EXPECTF--
int(42)
int(1)
===DONE===