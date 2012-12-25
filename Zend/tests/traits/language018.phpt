--TEST--
abstract alias
--FILE--
<?php
trait T1 {
	function foo() {}
}
class C1 {
	use T1 {
		T1::foo as abstract;
	}
}
?>
--EXPECTF--
Fatal error: Cannot use 'abstarct' as method modifier in %s on line %d
