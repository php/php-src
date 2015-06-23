--TEST--
Calling non-static method with call_user_func()
--FILE--
<?php

class foo {
	public function teste() {
		$this->a = 1;
	}
}

call_user_func(array('foo', 'teste'));

?>
--EXPECTF--
Deprecated: %son-static method foo::teste() should not be called statically in %s on line %d

Fatal error: Uncaught Error: Using $this when not in object context in %s:%d
Stack trace:
#0 %s(%d): foo::teste()
#1 {main}
  thrown in %s on line %d
