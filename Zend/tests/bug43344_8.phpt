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
Fatal error: Uncaught Error: Undefined constant 'Foo\bar' in %sbug43344_8.php:%d
Stack trace:
#0 %s(%d): Foo\f()
#1 {main}
  thrown in %sbug43344_8.php on line %d
