--TEST--
Class private constant visibility error
--FILE--
<?php
class A {
	private const privateConst = 'privateConst';
}

var_dump(A::privateConst);

?>
--EXPECTF--
Fatal error: Cannot access private const A::privateConst in %s on line 6
