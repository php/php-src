--TEST--
filter_var_array() and different arguments
--EXTENSIONS--
filter
--FILE--
<?php

echo "-- (1)\n";
var_dump(filter_var_array(array()));
var_dump(filter_var_array(array(1,"blah"=>"hoho")));
var_dump(filter_var_array(array(), -1));
var_dump(filter_var_array(array(), 1000000));

try {
    filter_var_array(array(), "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "-- (2)\n";
var_dump(filter_var_array(array(""=>""), -1));
var_dump(filter_var_array(array(""=>""), 1000000));

try {
    filter_var_array(array(""=>""), "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "-- (3)\n";
var_dump(filter_var_array(array("aaa"=>"bbb"), -1));
var_dump(filter_var_array(array("aaa"=>"bbb"), 1000000));

try {
    filter_var_array(array("aaa"=>"bbb"), "");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "-- (4)\n";

try {
    filter_var_array(array(), new stdclass);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(filter_var_array(array(), array()));
var_dump(filter_var_array(array(), array("var_name"=>1)));
var_dump(filter_var_array(array(), array("var_name"=>-1)));
var_dump(filter_var_array(array("var_name"=>""), array("var_name"=>-1)));

echo "-- (5)\n";
try {
    filter_var_array(array("var_name"=>""), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
try {
    filter_var_array(array("asdas"=>"text"), array("var_name"=>-1, "asdas"=>"asdasd", "qwe"=>"rty", ""=>""));
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$a = array(""=>""); $b = -1;
var_dump(filter_var_array($a, $b));
var_dump($a, $b);

$a = array(""=>""); $b = 100000;
var_dump(filter_var_array($a, $b));
var_dump($a, $b);

$a = array(""=>""); $b = "";
try {
    filter_var_array($a, $b);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
var_dump($a, $b);

echo "Done\n";
?>
--EXPECTF--
-- (1)
array(0) {
}
array(2) {
  [0]=>
  string(1) "1"
  ["blah"]=>
  string(4) "hoho"
}

Warning: filter_var_array(): Unknown filter with ID -1 in %s on line %d
bool(false)

Warning: filter_var_array(): Unknown filter with ID 1000000 in %s on line %d
bool(false)
filter_var_array(): Argument #2 ($options) must be of type array|int, string given
-- (2)

Warning: filter_var_array(): Unknown filter with ID -1 in %s on line %d
bool(false)

Warning: filter_var_array(): Unknown filter with ID 1000000 in %s on line %d
bool(false)
filter_var_array(): Argument #2 ($options) must be of type array|int, string given
-- (3)

Warning: filter_var_array(): Unknown filter with ID -1 in %s on line %d
bool(false)

Warning: filter_var_array(): Unknown filter with ID 1000000 in %s on line %d
bool(false)
filter_var_array(): Argument #2 ($options) must be of type array|int, string given
-- (4)
filter_var_array(): Argument #2 ($options) must be of type array|int, stdClass given
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
filter_var_array(): Argument #2 ($options) cannot contain empty keys
filter_var_array(): Argument #2 ($options) cannot contain empty keys

Warning: filter_var_array(): Unknown filter with ID -1 in %s on line %d
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
int(-1)

Warning: filter_var_array(): Unknown filter with ID 100000 in %s on line %d
bool(false)
array(1) {
  [""]=>
  string(0) ""
}
int(100000)
filter_var_array(): Argument #2 ($options) must be of type array|int, string given
array(1) {
  [""]=>
  string(0) ""
}
string(0) ""
Done
