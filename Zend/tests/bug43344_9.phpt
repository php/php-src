--TEST--
Bug #43344.9 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f($a=array(namespace\bar=>0)) {
	reset($a);
	return key($a);
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Undefined constant 'Foo\bar' in %sbug43344_9.php on line %d
