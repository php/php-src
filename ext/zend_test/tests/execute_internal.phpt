--TEST--
Test zend_execute_internal being called
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.execute_internal=1
--FILE--
<?php

function f($a) {
    var_dump(array_sum($a));
}

f(time() > 0 ? [1, 2, 3] : []);

?>
--EXPECT--
<!-- internal enter time() -->
<!-- internal enter array_sum() -->
<!-- internal enter var_dump() -->
int(6)
