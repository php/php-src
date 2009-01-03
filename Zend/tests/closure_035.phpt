--TEST--
Closure 035: Rebinding closure $this on property access
--FILE--
<?php

$instance = 0;

class Test {
	function __construct() {
		global $instance;
		$this->instance = ++$instance;
	}
}

$o = new Test;
$o->func = function () {
	var_dump($this);
};
$func = $o->func;
$func();

var_dump($instance);
?>
===DONE===
--EXPECTF--
object(Test)#%d (2) {
  ["instance"]=>
  int(1)
  ["func"]=>
  object(Closure)#%d (1) {
    ["this"]=>
    object(Test)#%d (2) {
      ["instance"]=>
      int(1)
      ["func"]=>
      object(Closure)#2 (1) {
        ["this"]=>
        *RECURSION*
      }
    }
  }
}
int(1)
===DONE===