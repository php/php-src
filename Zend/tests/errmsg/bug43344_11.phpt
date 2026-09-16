--TEST--
Bug #43344.11 (Wrong error message for undefined namespace constant)
--FILE--
<?php
function f($a=namespace\bar) {
    return $a;
}
echo f()."\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "bar" in %s:%d
Stack trace:
#0 %s(%d): f()
#1 {main}
  thrown in %sbug43344_11.php on line %d
