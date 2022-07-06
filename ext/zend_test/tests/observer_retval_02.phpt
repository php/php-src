--TEST--
Observer: Unused retvals from generators are still observable
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo() {
    yield 'I should be observable';
    yield 'Me too!';
}

$gen = foo();
$gen->current();
$gen->next();
$gen->current();

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_retval_%d.php' -->
<file '%s%eobserver_retval_%d.php'>
  <!-- init foo() -->
  <foo>
  </foo:'I should be observable'>
  <foo>
  </foo:'Me too!'>
Done
</file '%s%eobserver_retval_%d.php'>
