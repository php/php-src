--TEST--
ReflectionMethod::__construct() tests
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

$a = array("", 1, "::", "a::", "::b", "a::b");

foreach ($a as $val) {
	try {
		new ReflectionMethod($val);
	} catch (Exception $e) {
		var_dump($e->getMessage());
	}
}
 
$a = array("", 1, "");
$b = array("", "", 1);
 
foreach ($a as $key=>$val) {
	try {
		new ReflectionMethod($val, $b[$key]);
	} catch (Exception $e) {
		var_dump($e->getMessage());
	}
}

echo "Done\n";
?>
--EXPECTF--	
string(20) "Invalid method name "
string(21) "Invalid method name 1"
string(21) "Class  does not exist"
string(22) "Class a does not exist"
string(21) "Class  does not exist"
string(22) "Class a does not exist"
string(21) "Class  does not exist"
string(66) "The parameter class is expected to be either a string or an object"
string(21) "Class  does not exist"
Done
