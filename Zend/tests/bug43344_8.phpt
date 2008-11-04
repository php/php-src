--TEST--
Bug #43344.8 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f($a=array(namespace\bar)) {
	return $a[0];
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Undefined constant 'Foo\bar' in %sbug43344_8.php on line %d
