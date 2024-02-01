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
<!-- init '%s' -->
<file '%s'>
  <!-- init array_reduce() -->
  <array_reduce>
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
  </array_reduce>
  <!-- init var_dump() -->
  <var_dump>
int(15)
  </var_dump>
Done
</file '%s'>
