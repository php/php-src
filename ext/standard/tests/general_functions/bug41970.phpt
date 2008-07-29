--TEST--
Bug #41970 (call_user_func_*() leaks on failure)
--FILE--
<?php

$a = array(4,3,2);

var_dump(call_user_func_array("sort", array($a)));
var_dump(call_user_func_array("strlen", array($a)));
var_dump(call_user_func("sort", $a));
var_dump(call_user_func("strlen", $a));

echo "Done\n";
?>
--EXPECTF--
Warning: Parameter 1 to sort() expected to be a reference, value given in %s on line %d
NULL

Warning: strlen() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL

Warning: Parameter 1 to sort() expected to be a reference, value given in %s on line %d
NULL

Warning: strlen() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
Done
