--TEST--
Type check in call_user_func_array() + array_slice() optimization
--FILE--
<?php

$array = [1, 2, 3];

try {
    $len = [];
    call_user_func_array('var_dump', array_slice($array, 0, $len));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$len = 2.0;
call_user_func_array('var_dump', array_slice($array, 0, $len));

$len = null;
call_user_func_array('var_dump', array_slice($array, 1, $len));

?>
--EXPECT--
array_slice(): Argument #3 ($length) must be of type ?int, array given
int(1)
int(2)
int(2)
int(3)
