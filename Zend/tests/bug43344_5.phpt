--TEST--
Bug #43344.5 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f($a=array(Foo::bar=>0)) {
	reset($a);
	return key($a);
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class 'Foo\Foo' not found in %s:%d
Stack trace:
#0 %s(%d): Foo\f()
#1 {main}
  thrown in %s on line %d
