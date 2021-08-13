--TEST--
Observer: Retvals by reference are observable that are: IS_CV
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function &foo() {
    $retval = 'I should be observable';
    return $retval; // IS_CV
}

$res = foo(); // Retval used
foo(); // Retval unused

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_retval_by_ref_%d.php' -->
<file '%s%eobserver_retval_by_ref_%d.php'>
  <!-- init foo() -->
  <foo>
  </foo:'I should be observable'>
  <foo>
  </foo:'I should be observable'>
Done
</file '%s%eobserver_retval_by_ref_%d.php'>
