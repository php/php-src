--TEST--
Test sort() function : usage variations - sort strings
--FILE--
<?php

$array = [
    "lemoN",
    "Orange",
    "banana",
    "apple",
    "Test",
    "TTTT",
    "ttt",
    "ww",
    "x",
    "X",
    "oraNGe",
    "BANANA",
];

echo "Default flag\n";
$temp_array = $array;
var_dump(sort($temp_array)); // expecting : bool(true)
var_dump($temp_array);

echo "SORT_REGULAR\n";
$temp_array = $array;
var_dump(sort($temp_array, SORT_REGULAR)); // expecting : bool(true)
var_dump($temp_array);

echo "SORT_STRING\n";
$temp_array = $array;
var_dump(sort($temp_array, SORT_STRING)); // expecting : bool(true)
var_dump($temp_array);

?>
--EXPECT--
Default flag
bool(true)
array(12) {
  [0]=>
  string(6) "BANANA"
  [1]=>
  string(6) "Orange"
  [2]=>
  string(4) "TTTT"
  [3]=>
  string(4) "Test"
  [4]=>
  string(1) "X"
  [5]=>
  string(5) "apple"
  [6]=>
  string(6) "banana"
  [7]=>
  string(5) "lemoN"
  [8]=>
  string(6) "oraNGe"
  [9]=>
  string(3) "ttt"
  [10]=>
  string(2) "ww"
  [11]=>
  string(1) "x"
}
SORT_REGULAR
bool(true)
array(12) {
  [0]=>
  string(6) "BANANA"
  [1]=>
  string(6) "Orange"
  [2]=>
  string(4) "TTTT"
  [3]=>
  string(4) "Test"
  [4]=>
  string(1) "X"
  [5]=>
  string(5) "apple"
  [6]=>
  string(6) "banana"
  [7]=>
  string(5) "lemoN"
  [8]=>
  string(6) "oraNGe"
  [9]=>
  string(3) "ttt"
  [10]=>
  string(2) "ww"
  [11]=>
  string(1) "x"
}
SORT_STRING
bool(true)
array(12) {
  [0]=>
  string(6) "BANANA"
  [1]=>
  string(6) "Orange"
  [2]=>
  string(4) "TTTT"
  [3]=>
  string(4) "Test"
  [4]=>
  string(1) "X"
  [5]=>
  string(5) "apple"
  [6]=>
  string(6) "banana"
  [7]=>
  string(5) "lemoN"
  [8]=>
  string(6) "oraNGe"
  [9]=>
  string(3) "ttt"
  [10]=>
  string(2) "ww"
  [11]=>
  string(1) "x"
}
