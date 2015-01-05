--TEST--
forward_static_call() from outside of a class method.
--FILE--
<?php

class A
{
	public static function test() {
		echo "A\n";
	}
}

function test() {
	forward_static_call(array('A', 'test'));
}

test();

?>
--EXPECTF--
Fatal error: Cannot call forward_static_call() when no class scope is active in %s on line %d
