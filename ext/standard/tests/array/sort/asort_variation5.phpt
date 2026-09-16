--TEST--
Test asort() function : usage variations - sort strings
--FILE--
<?php

$array = [
    "lemoN" => "lemoN",
    "Orange" => "Orange",
    "banana" => "banana",
    "apple" => "apple",
    "Test" => "Test",
    "TTTT" => "TTTT",
    "ttt" => "ttt",
    "ww" => "ww",
    "x" => "x",
    "X" => "X",
    "oraNGe" => "oraNGe",
    "BANANA" => "BANANA",
];

echo "Default flag\n";
$temp_array = $array;
var_dump(asort($temp_array)); // expecting : bool(true)
var_dump($temp_array);

echo "SORT_REGULAR\n";
$temp_array = $array;
var_dump(asort($temp_array, SORT_REGULAR)); // expecting : bool(true)
var_dump($temp_array);

echo "SORT_STRING\n";
$temp_array = $array;
var_dump(asort($temp_array, SORT_STRING)); // expecting : bool(true)
var_dump($temp_array);

?>
--EXPECT--
Default flag
bool(true)
array(12) {
  ["BANANA"]=>
  string(6) "BANANA"
  ["Orange"]=>
  string(6) "Orange"
  ["TTTT"]=>
  string(4) "TTTT"
  ["Test"]=>
  string(4) "Test"
  ["X"]=>
  string(1) "X"
  ["apple"]=>
  string(5) "apple"
  ["banana"]=>
  string(6) "banana"
  ["lemoN"]=>
  string(5) "lemoN"
  ["oraNGe"]=>
  string(6) "oraNGe"
  ["ttt"]=>
  string(3) "ttt"
  ["ww"]=>
  string(2) "ww"
  ["x"]=>
  string(1) "x"
}
SORT_REGULAR
bool(true)
array(12) {
  ["BANANA"]=>
  string(6) "BANANA"
  ["Orange"]=>
  string(6) "Orange"
  ["TTTT"]=>
  string(4) "TTTT"
  ["Test"]=>
  string(4) "Test"
  ["X"]=>
  string(1) "X"
  ["apple"]=>
  string(5) "apple"
  ["banana"]=>
  string(6) "banana"
  ["lemoN"]=>
  string(5) "lemoN"
  ["oraNGe"]=>
  string(6) "oraNGe"
  ["ttt"]=>
  string(3) "ttt"
  ["ww"]=>
  string(2) "ww"
  ["x"]=>
  string(1) "x"
}
SORT_STRING
bool(true)
array(12) {
  ["BANANA"]=>
  string(6) "BANANA"
  ["Orange"]=>
  string(6) "Orange"
  ["TTTT"]=>
  string(4) "TTTT"
  ["Test"]=>
  string(4) "Test"
  ["X"]=>
  string(1) "X"
  ["apple"]=>
  string(5) "apple"
  ["banana"]=>
  string(6) "banana"
  ["lemoN"]=>
  string(5) "lemoN"
  ["oraNGe"]=>
  string(6) "oraNGe"
  ["ttt"]=>
  string(3) "ttt"
  ["ww"]=>
  string(2) "ww"
  ["x"]=>
  string(1) "x"
}