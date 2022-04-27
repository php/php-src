--TEST--
Observer: Retvals by reference are observable that are: IS_VAR, ZEND_RETURNS_FUNCTION
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
opcache.optimization_level=0
--FILE--
<?php
function getMessage() {
  return 'I should be observable';
}

function &foo() {
    return getMessage(); // IS_VAR + ZEND_RETURNS_FUNCTION
}

$res = foo(); // Retval used
foo(); // Retval unused

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_retval_by_ref_%d.php' -->
<file '%s%eobserver_retval_by_ref_%d.php'>
  <!-- init foo() -->
  <foo>
    <!-- init getMessage() -->
    <getMessage>
    </getMessage:'I should be observable'>

Notice: Only variable references should be returned by reference in %s on line %d
  </foo:'I should be observable'>
  <foo>
    <getMessage>
    </getMessage:'I should be observable'>

Notice: Only variable references should be returned by reference in %s on line %d
  </foo:'I should be observable'>
Done
</file '%s%eobserver_retval_by_ref_%d.php'>
