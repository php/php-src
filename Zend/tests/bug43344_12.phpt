--TEST--
Bug #43344.12 (Wrong error message for undefined namespace constant)
--FILE--
<?php
function f($a=array(namespace\bar)) {
	return $a[0];
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant 'bar' in %sbug43344_12.php:%d
Stack trace:
#0 %s(%d): f()
#1 {main}
  thrown in %sbug43344_12.php on line %d
