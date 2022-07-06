--TEST--
Observer: Basic eval observability
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
echo eval("return 'Foo eval' . PHP_EOL;");
echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_eval_%d.php' -->
<file '%s%eobserver_eval_%d.php'>
  <!-- init '%s%eobserver_eval_%d.php(%d) : eval()'d code' -->
  <file '%s%eobserver_eval_%d.php(%d) : eval()'d code'>
  </file '%s%eobserver_eval_%d.php(%d) : eval()'d code'>
Foo eval
DONE
</file '%s%eobserver_eval_%d.php'>
