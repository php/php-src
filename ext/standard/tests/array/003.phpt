--TEST--
Test usort, uksort and uasort
--POST--
--GET--
--INI--
precision=14
--FILE--
<?php
require('ext/standard/tests/array/data.inc');

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
array(7) {
  [-2147483648]=>
  float(-0.33333333333333)
  ["-2147483648"]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  ["test"]=>
  int(27)
  [0]=>
  string(3) "PHP"
  [17]=>
  string(27) "PHP: Hypertext Preprocessor"
  [5]=>
  string(4) "Test"
  [2147483647]=>
  string(4) "test"
}

 -- Testing uksort() -- 
array(7) {
  ["-2147483648"]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  [-2147483648]=>
  float(-0.33333333333333)
  [0]=>
  string(3) "PHP"
  [17]=>
  string(27) "PHP: Hypertext Preprocessor"
  [2147483647]=>
  string(4) "test"
  [5]=>
  string(4) "Test"
  ["test"]=>
  int(27)
}

 -- Testing usort() -- 
array(7) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
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
  string(4) "test"
}
