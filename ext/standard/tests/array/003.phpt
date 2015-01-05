--TEST--
Test usort, uksort and uasort
--INI--
precision=14
--FILE--
<?php
require(dirname(__FILE__) . '/data.inc');

function cmp ($a, $b) {
    is_array ($a)
        and $a = array_sum ($a);
    is_array ($b)
        and $b = array_sum ($b);
    return strcmp ($a, $b);
}

echo " -- Testing uasort() -- \n";
uasort ($data, 'cmp');
var_dump ($data);


echo "\n -- Testing uksort() -- \n";
uksort ($data, 'cmp');
var_dump ($data);

echo "\n -- Testing usort() -- \n";
usort ($data, 'cmp');
var_dump ($data);
?>
--EXPECT--
-- Testing uasort() -- 
array(8) {
  [16777216]=>
  float(-0.33333333333333)
  [-1000]=>
  array(2) {
    [0]=>
    string(6) "banana"
    [1]=>
    string(6) "orange"
  }
  ["test"]=>
  int(27)
  [0]=>
  string(3) "PHP"
  [17]=>
  string(27) "PHP: Hypertext Preprocessor"
  [5]=>
  string(4) "Test"
  [1001]=>
  string(6) "monkey"
  [1000]=>
  string(4) "test"
}

 -- Testing uksort() -- 
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    string(6) "banana"
    [1]=>
    string(6) "orange"
  }
  [0]=>
  string(3) "PHP"
  [1000]=>
  string(4) "test"
  [1001]=>
  string(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
  [17]=>
  string(27) "PHP: Hypertext Preprocessor"
  [5]=>
  string(4) "Test"
  ["test"]=>
  int(27)
}

 -- Testing usort() -- 
array(8) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  array(2) {
    [0]=>
    string(6) "banana"
    [1]=>
    string(6) "orange"
  }
  [2]=>
  int(27)
  [3]=>
  string(3) "PHP"
  [4]=>
  string(27) "PHP: Hypertext Preprocessor"
  [5]=>
  string(4) "Test"
  [6]=>
  string(6) "monkey"
  [7]=>
  string(4) "test"
}
