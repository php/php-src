--TEST--
Test zend_execute_internal being called
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.execute_internal=1
zend_test.observer.show_return_value=1
--FILE--
<?php

function f($a) {
    var_dump(array_sum($a));
}

f(time() > 0 ? [1, 2, 3] : []);

?>
--EXPECTF--
<!-- internal enter time() -->
<!-- internal leave time():%d -->
<!-- internal enter array_sum() -->
<!-- internal leave array_sum():6 -->
<!-- internal enter var_dump() -->
int(6)
<!-- internal leave var_dump():NULL -->
