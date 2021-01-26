--TEST--
Observer: End handlers fire after a userland fatal error
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    trigger_error('Foo error', E_USER_ERROR);
});

function foo()
{
	return $x; // warning
}

foo();

echo 'You should not see this.';
?>
--EXPECTF--
<!-- init '%s%eobserver_error_%d.php' -->
<file '%s%eobserver_error_%d.php'>
  <!-- init foo() -->
  <foo>
    <!-- init {closure}() -->
    <{closure}>

Fatal error: Foo error in %s on line %d
    </{closure}:NULL>
  </foo:NULL>
</file '%s%eobserver_error_%d.php'>
