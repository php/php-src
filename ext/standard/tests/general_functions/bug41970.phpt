--TEST--
Bug #41970 (call_user_func_*() leaks on failure)
--FILE--
<?php

$a = array(4,3,2);

var_dump(call_user_func_array("sort", array($a)));
try {
    var_dump(call_user_func_array("strlen", array($a)));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(call_user_func("sort", $a));
try {
    var_dump(call_user_func("strlen", $a));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo "Done\n";
?>
--EXPECTF--
Warning: sort() expects argument #1 ($arg) to be passed by reference, value given in %s on line %d
bool(true)
strlen() expects argument #1 ($str) to be of type string, array given

Warning: sort() expects argument #1 ($arg) to be passed by reference, value given in %s on line %d
bool(true)
strlen() expects argument #1 ($str) to be of type string, array given
Done
