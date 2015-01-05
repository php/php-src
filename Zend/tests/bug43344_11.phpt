--TEST--
Bug #43344.11 (Wrong error message for undefined namespace constant)
--FILE--
<?php
function f($a=namespace\bar) {
	return $a;
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Undefined constant 'bar' in %sbug43344_11.php on line %d
