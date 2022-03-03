--TEST--
Observer: Retvals are observable that are: IS_REFERENCE, IS_VAR
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function &getMessage() {
    $retval = 'I should be observable';
    return $retval;
}

function foo() {
    return getMessage(); // IS_REFERENCE + IS_VAR
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
    <!-- init getMessage() -->
    <getMessage>
    </getMessage:'I should be observable'>
  </foo:'I should be observable'>
  <foo>
    <getMessage>
    </getMessage:'I should be observable'>
  </foo:'I should be observable'>
Done
</file '%s%eobserver_retval_%d.php'>
