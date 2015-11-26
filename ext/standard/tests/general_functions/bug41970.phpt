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
Warning: Parameter 1 to sort() expected to be a reference, value given in %sbug41970.php on line 5
NULL

Warning: strlen() expects parameter 1 to be string, array given in %sbug41970.php on line 6
NULL

Warning: Parameter 1 to sort() expected to be a reference, value given in %sbug41970.php on line 7
NULL

Warning: strlen() expects parameter 1 to be string, array given in %sbug41970.php on line 8
NULL
Done
