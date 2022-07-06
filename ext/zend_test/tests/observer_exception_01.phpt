--TEST--
Observer: Basic observability of userland functions with uncaught exceptions
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
function foo()
{
    static $callCount = 0;
    echo 'Call #' . $callCount . PHP_EOL;
    if (++$callCount == 3) {
        throw new RuntimeException('Third time is a charm');
    }
}

foo();
foo();
foo();

echo 'You should not see this' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_exception_%d.php' -->
<file '%s%eobserver_exception_%d.php'>
  <!-- init foo() -->
  <foo>
Call #0
  </foo>
  <foo>
Call #1
  </foo>
  <foo>
Call #2
    <!-- Exception: RuntimeException -->
  </foo>
  <!-- Exception: RuntimeException -->
</file '%s%eobserver_exception_%d.php'>

Fatal error: Uncaught RuntimeException: Third time is a charm in %s%eobserver_exception_%d.php:%d
Stack trace:
#0 %s%eobserver_exception_%d.php(%d): foo()
#1 {main}
  thrown in %s%eobserver_exception_%d.php on line %d
