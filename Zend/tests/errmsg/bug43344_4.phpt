--TEST--
Bug #43344.4 (Wrong error message for undefined namespace constant)
--FILE--
<?php
namespace Foo;
function f($a=array(Foo::bar)) {
    return $a[0];
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class "Foo\Foo" not found in %s:%d
Stack trace:
#0 %s(%d): Foo\f()
#1 {main}
  thrown in %s on line %d
