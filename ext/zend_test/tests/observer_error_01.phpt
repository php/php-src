--TEST--
Observer: End handlers fire after a fatal error
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
memory_limit=1M
--FILE--
<?php
function foo()
{
    str_repeat('.', 1024 * 1024 * 2); // 2MB
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s%eobserver_error_%d.php' -->
<file '%s%eobserver_error_%d.php'>
  <!-- init foo() -->
  <foo>

Fatal error: Allowed memory size of 2097152 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
  </foo:NULL>
</file '%s%eobserver_error_%d.php'>
