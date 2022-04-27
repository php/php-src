--TEST--
Observer: Calls that go through zend_call_function are observed
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php
function sum($carry, $item) {
    $carry += $item;
    return $carry;
}

$a = [1, 2, 3, 4, 5];
// array_reduce() calls zend_call_function() under the hood
var_dump(array_reduce($a, 'sum'));
echo 'Done' . PHP_EOL;
?>
--EXPECTF--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
<!-- init '%s%eobserver_zend_call_function_%d.php' -->
<file '%s%eobserver_zend_call_function_%d.php'>
  <!-- init sum() -->
  <sum>
  </sum>
  <sum>
  </sum>
  <sum>
  </sum>
  <sum>
  </sum>
  <sum>
  </sum>
int(15)
Done
</file '%s%eobserver_zend_call_function_%d.php'>
