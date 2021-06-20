--TEST--
Observer: Retvals are observable that are: IS_CONST
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
opcache.optimization_level=0
--FILE--
<?php
function foo() {
    return 'I should be observable'; // IS_CONST
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
  </foo:'I should be observable'>
  <foo>
  </foo:'I should be observable'>
Done
</file '%s%eobserver_retval_%d.php'>
