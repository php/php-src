--TEST--
filter_assert_array() and different arguments
--SKIPIF--
<?php if (!extension_loaded("filter")) print "skip"; ?>
--FILE--
<?php

echo "-- (1)\n";
try {
	var_dump(filter_assert_array(NULL));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array()));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(1,"blah"=>"hoho")));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(), -1));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(), 1000000));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(), ""));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (2)\n";
try {
	var_dump(filter_assert_array(array(""=>""), -1));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(""=>""), 1000000));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(""=>""), ""));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (3)\n";
try {
	var_dump(filter_assert_array(array("aaa"=>"bbb"), -1));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array("aaa"=>"bbb"), 1000000));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array("aaa"=>"bbb"), ""));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (4)\n";
try {
	var_dump(filter_assert_array(array(), new stdclass));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(), array()));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(), array("var_name"=>1)));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array(), array("var_name"=>-1)));
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	var_dump(filter_assert_array(array("var_name"=>""), array("var_name"=>-1)));
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "-- (5)\n";
try {
	var_dump(filter_assert_array(array("var_name"=>""), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>"")));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

try {
var_dump(filter_assert_array(array("asdas"=>"text"), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>"")));
} catch (Exception $e) {
	var_dump($e->getMessage());
}

try {
	$a = array(""=>""); $b = -1;
	var_dump(filter_assert_array($a, $b));
	var_dump($a, $b);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

try {
	$a = array(""=>""); $b = 100000;
	var_dump(filter_assert_array($a, $b));
	var_dump($a, $b);
} catch (Exception $e) {
	var_dump($e->getMessage());
}

try {
	$a = array(""=>""); $b = "";
	var_dump(filter_assert_array($a, $b));
	var_dump($a, $b);
} catch (Exception $e) {
	var_dump($e->getMessage());
}


echo "Done\n";
?>
--EXPECTF--
-- (1)

Warning: filter_assert_array() expects at least 2 parameters, 1 given in %s on line 5
NULL

Warning: filter_assert_array() expects at least 2 parameters, 1 given in %s on line 10
NULL

Warning: filter_assert_array() expects at least 2 parameters, 1 given in %s on line 15
NULL

Warning: filter_assert_array(): Invalid option parameter in %s on line 20
bool(false)

Warning: filter_assert_array(): Invalid option parameter in %s on line 25
bool(false)

Warning: filter_assert_array(): Invalid option parameter in %s on line 30
bool(false)
-- (2)

Warning: filter_assert_array(): Invalid option parameter in %s on line 38
bool(false)

Warning: filter_assert_array(): Invalid option parameter in %s on line 43
bool(false)

Warning: filter_assert_array(): Invalid option parameter in %s on line 48
bool(false)
-- (3)

Warning: filter_assert_array(): Invalid option parameter in %s on line 56
bool(false)

Warning: filter_assert_array(): Invalid option parameter in %s on line 61
bool(false)

Warning: filter_assert_array(): Invalid option parameter in %s on line 66
bool(false)
-- (4)

Warning: filter_assert_array(): Invalid option parameter in %s on line 74
bool(false)
array(0) {
}
string(37) "Filter validated value does not exist"
string(37) "Filter validated value does not exist"
array(1) {
  ["var_name"]=>
  string(0) ""
}
-- (5)

Warning: filter_assert_array(): Empty keys are not allowed in the definition array in %s on line 102
string(37) "Filter validated value does not exist"

Warning: filter_assert_array(): Invalid filter specifieid (0) in %s on line 108

Warning: filter_assert_array(): Empty keys are not allowed in the definition array in %s on line 108
string(37) "Filter validated value does not exist"

Warning: filter_assert_array(): Invalid option parameter in %s on line 115
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
int(-1)

Warning: filter_assert_array(): Invalid option parameter in %s on line 123
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
int(100000)

Warning: filter_assert_array(): Invalid option parameter in %s on line 131
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
string(0) ""
Done
