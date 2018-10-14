--TEST--
Abstract constants are not allowed
--FILE--
<?php
class A {
	abstract const X = 1;
}
?>
--EXPECTF--
Fatal error: Cannot use 'abstract' as constant modifier in %s on line 3
