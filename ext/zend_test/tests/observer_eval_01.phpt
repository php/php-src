--TEST--
Observer: Basic eval observability
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
echo eval("return 'Foo eval' . PHP_EOL;");
echo 'DONE' . PHP_EOL;
?>
--EXPECTF--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_eval_%d.php' -->
<file '%s%eobserver_eval_%d.php'>
  <!-- init '%s%eobserver_eval_%d.php(%d) : eval()'d code' -->
  <file '%s%eobserver_eval_%d.php(%d) : eval()'d code'>
  </file '%s%eobserver_eval_%d.php(%d) : eval()'d code'>
Foo eval
DONE
</file '%s%eobserver_eval_%d.php'>
