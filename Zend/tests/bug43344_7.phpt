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
Fatal error: Uncaught Error: Undefined constant 'Foo\bar' in %sbug43344_7.php:%d
Stack trace:
#0 %s(%d): Foo\f()
#1 {main}
  thrown in %sbug43344_7.php on line %d
