--TEST--
Default parameters - 8, by-refs
--FILE--
<?php

var_dump(is_callable("is_callable", default, default));
$test = "";
var_dump(is_callable("is_not_callable", default, $test));
var_dump($test);
echo "Done\n";
?>
--EXPECTF--	
bool(true)
bool(false)
string(15) "is_not_callable"
Done
