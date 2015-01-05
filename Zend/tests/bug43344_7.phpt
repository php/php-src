--TEST--
Bug #43344.7 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f($a=namespace\bar) {
	return $a;
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Undefined constant 'Foo\bar' in %sbug43344_7.php on line %d
