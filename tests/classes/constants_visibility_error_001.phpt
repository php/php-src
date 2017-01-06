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
Fatal error: Uncaught Error: Cannot access private const A::privateConst in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6
