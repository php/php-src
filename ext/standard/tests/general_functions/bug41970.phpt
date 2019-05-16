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
Warning: Parameter 1 to sort() expected to be a reference, value given in %sbug41970.php on line %d
bool(true)
strlen() expects parameter 1 to be string, array given

Warning: Parameter 1 to sort() expected to be a reference, value given in %sbug41970.php on line %d
bool(true)
strlen() expects parameter 1 to be string, array given
Done
