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
NULL

Notice: Array to string conversion in %s on line %d
int(5)
NULL

Notice: Array to string conversion in %s on line %d
int(5)
Done
