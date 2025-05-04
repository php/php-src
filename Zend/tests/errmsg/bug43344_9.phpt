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
Fatal error: Uncaught Error: Undefined constant "Foo\bar" in %s:%d
Stack trace:
#0 %s(%d): Foo\f()
#1 {main}
  thrown in %sbug43344_9.php on line %d
