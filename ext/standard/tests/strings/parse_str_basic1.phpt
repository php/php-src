--TEST--
Test parse_str() function : basic functionality
--FILE--
<?php

echo "*** Testing parse_str() : basic functionality ***\n";

echo "\nBasic test WITH undefined var for result arg\n";
$s1 = "first=val1&second=val2&third=val3";
var_dump(parse_str($s1, $res1));
var_dump($res1);

echo "\nBasic test WITH existing non-array var for result arg\n";
$res2 =99;
$s1 = "first=val1&second=val2&third=val3";
var_dump(parse_str($s1, $res2));
var_dump($res2);

echo "\nBasic test with an existing array as results array\n";
$res3_array = array(1,2,3,4);
var_dump(parse_str($s1, $res3_array));
var_dump($res3_array);

?>
--EXPECT--
*** Testing parse_str() : basic functionality ***

Basic test WITH undefined var for result arg
NULL
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}

Basic test WITH existing non-array var for result arg
NULL
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}

Basic test with an existing array as results array
NULL
array(3) {
  ["first"]=>
  string(4) "val1"
  ["second"]=>
  string(4) "val2"
  ["third"]=>
  string(4) "val3"
}
