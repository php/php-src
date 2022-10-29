--TEST--
Observer: Retvals are observable that are: IS_CV, IS_UNDEF
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function foo() {
    return $i_do_not_exist; // IS_CV && IS_UNDEF
}

$res = foo(); // Retval used
foo(); // Retval unused

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_retval_%d.php' -->
<file '%s%eobserver_retval_%d.php'>
  <!-- init foo() -->
  <foo>

Warning: Undefined variable $i_do_not_exist in %s on line %d
  </foo:NULL>
  <foo>

Warning: Undefined variable $i_do_not_exist in %s on line %d
  </foo:NULL>
Done
</file '%s%eobserver_retval_%d.php'>
