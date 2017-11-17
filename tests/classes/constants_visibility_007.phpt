--TEST--
Final constants are not allowed
--FILE--
<?php
class A {
	final const X = 1;
}
?>
--EXPECTF--
Fatal error: Cannot use 'final' as constant modifier in %s on line 3
