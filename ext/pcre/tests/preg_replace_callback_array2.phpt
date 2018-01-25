--TEST--
preg_replace_callback_array() errors
--FILE--
<?php

var_dump(preg_replace_callback_array());
var_dump(preg_replace_callback_array(1));
var_dump(preg_replace_callback_array(1,2));
var_dump(preg_replace_callback_array(1,2,3));
$a = 5;
var_dump(preg_replace_callback_array(1,2,3,$a));
$a = "";
var_dump(preg_replace_callback_array(array("" => ""),"","",$a));
$a = array();
$b = "";
var_dump(preg_replace_callback($a, $a, $a, $a, $b));
var_dump($b);
$b = "";
var_dump(preg_replace_callback_array(array("xx" => "s"), $a, -1, $b));
var_dump($b);
function f() {
	static $count = 1;
	throw new Exception($count);
}

var_dump(preg_replace_callback_array(array('/\w' => 'f'), 'z'));

try {
	var_dump(preg_replace_callback_array(array('/\w/' => 'f', '/.*/' => 'f'), 'z'));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

echo "Done\n";
?>
--EXPECTF--
Warning: preg_replace_callback_array() expects at least 2 parameters, 0 given in %s on line %d
NULL

Warning: preg_replace_callback_array() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: preg_replace_callback_array() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: preg_replace_callback_array() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: preg_replace_callback_array() expects parameter 1 to be array, integer given in %s on line %d
NULL

Warning: preg_replace_callback_array() expects parameter 3 to be integer, string given in %s on line %d
NULL

Warning: preg_replace_callback() expects parameter 4 to be integer, array given in %s on line %d
NULL
string(0) ""

Warning: preg_replace_callback_array(): 's' is not a valid callback in %spreg_replace_callback_array2.php on line %d
array(0) {
}
string(0) ""

Warning: preg_replace_callback_array(): No ending delimiter '/' found in %spreg_replace_callback_array2.php on line %d
NULL
string(1) "1"
Done
