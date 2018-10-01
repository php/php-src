--TEST--
Bug #43344.3 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f($a=Foo::bar) {
	return $a;
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class 'Foo\Foo' not found in %s:%d
Stack trace:
#0 %s(%d): Foo\f()
#1 {main}
  thrown in %s on line %d
