--TEST--
Test arsort() function : usage variations - sort strings
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
var_dump(arsort($temp_array)); // expecting : bool(true)
var_dump($temp_array);

echo "SORT_REGULAR\n";
$temp_array = $array;
var_dump(arsort($temp_array, SORT_REGULAR)); // expecting : bool(true)
var_dump($temp_array);

echo "SORT_STRING\n";
$temp_array = $array;
var_dump(arsort($temp_array, SORT_STRING)); // expecting : bool(true)
var_dump($temp_array);

?>
--EXPECT--
Default flag
bool(true)
array(12) {
  ["x"]=>
  string(1) "x"
  ["ww"]=>
  string(2) "ww"
  ["ttt"]=>
  string(3) "ttt"
  ["oraNGe"]=>
  string(6) "oraNGe"
  ["lemoN"]=>
  string(5) "lemoN"
  ["banana"]=>
  string(6) "banana"
  ["apple"]=>
  string(5) "apple"
  ["X"]=>
  string(1) "X"
  ["Test"]=>
  string(4) "Test"
  ["TTTT"]=>
  string(4) "TTTT"
  ["Orange"]=>
  string(6) "Orange"
  ["BANANA"]=>
  string(6) "BANANA"
}
SORT_REGULAR
bool(true)
array(12) {
  ["x"]=>
  string(1) "x"
  ["ww"]=>
  string(2) "ww"
  ["ttt"]=>
  string(3) "ttt"
  ["oraNGe"]=>
  string(6) "oraNGe"
  ["lemoN"]=>
  string(5) "lemoN"
  ["banana"]=>
  string(6) "banana"
  ["apple"]=>
  string(5) "apple"
  ["X"]=>
  string(1) "X"
  ["Test"]=>
  string(4) "Test"
  ["TTTT"]=>
  string(4) "TTTT"
  ["Orange"]=>
  string(6) "Orange"
  ["BANANA"]=>
  string(6) "BANANA"
}
SORT_STRING
bool(true)
array(12) {
  ["x"]=>
  string(1) "x"
  ["ww"]=>
  string(2) "ww"
  ["ttt"]=>
  string(3) "ttt"
  ["oraNGe"]=>
  string(6) "oraNGe"
  ["lemoN"]=>
  string(5) "lemoN"
  ["banana"]=>
  string(6) "banana"
  ["apple"]=>
  string(5) "apple"
  ["X"]=>
  string(1) "X"
  ["Test"]=>
  string(4) "Test"
  ["TTTT"]=>
  string(4) "TTTT"
  ["Orange"]=>
  string(6) "Orange"
  ["BANANA"]=>
  string(6) "BANANA"
}
