--TEST--
Basic return hints callable
--FILE--
<?php
class foo {
	public function bar() : callable {
		return function() {};
	}
}

$baz = new foo();
var_dump($baz->bar());
?>
--EXPECTF--
object(Closure)#%d (%d) {
  ["this"]=>
  object(foo)#%d (0) {
  }
}


