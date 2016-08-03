--TEST--
validate_var_array() and different arguments
--SKIPIF--
<?php if (!extension_loaded("filter")) print "skip"; ?>
--FILE--
<?php

echo "-- (1)\n";
try {
	var_dump(validate_var_array(NULL));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array()));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(1,"blah"=>"hoho")));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(), -1));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(), 1000000));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(), ""));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (2)\n";
try {
	var_dump(validate_var_array(array(""=>""), -1));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(""=>""), 1000000));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(""=>""), ""));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (3)\n";
try {
	var_dump(validate_var_array(array("aaa"=>"bbb"), -1));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array("aaa"=>"bbb"), 1000000));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array("aaa"=>"bbb"), ""));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (4)\n";
try {
	var_dump(validate_var_array(array(), new stdclass));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(), array()));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(), array("var_name"=>1)));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array(), array("var_name"=>-1)));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(validate_var_array(array("var_name"=>""), array("var_name"=>-1)));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (5)\n";
var_dump(validate_var_array(array("var_name"=>""), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>"")));
var_dump(validate_var_array(array("asdas"=>"text"), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>"")));


$a = array(""=>""); $b = -1;
var_dump(validate_var_array($a, $b));
var_dump($a, $b);

$a = array(""=>""); $b = 100000;
var_dump(validate_var_array($a, $b));
var_dump($a, $b);

$a = array(""=>""); $b = "";
var_dump(validate_var_array($a, $b));
var_dump($a, $b);

echo "Done\n";
?>
--EXPECTF--
-- (1)

Warning: validate_var_array() expects parameter 1 to be array, null given in %s on line %d
NULL
string(37) "Filter validation rule does not exist"
string(37) "Filter validation rule does not exist"
bool(false)
bool(false)
bool(false)
-- (2)
bool(false)
bool(false)
bool(false)
-- (3)
bool(false)
bool(false)
bool(false)
-- (4)
bool(false)
array(0) {
}
array(1) {
  ["var_name"]=>
  NULL
}
array(1) {
  ["var_name"]=>
  NULL
}
array(1) {
  ["var_name"]=>
  string(0) ""
}
-- (5)

Warning: validate_var_array(): Empty keys are not allowed in the definition array in %s on line %d
bool(false)

Warning: validate_var_array(): Empty keys are not allowed in the definition array in %s on line %d
bool(false)
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
int(-1)
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
int(100000)
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
string(0) ""
Done
