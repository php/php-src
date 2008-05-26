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
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [u"test"]=>
  int(27)
  [0]=>
  unicode(3) "PHP"
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [1001]=>
  unicode(6) "monkey"
  [1000]=>
  unicode(4) "test"
}

 -- Testing uksort() -- 
array(8) {
  [-1000]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [0]=>
  unicode(3) "PHP"
  [1000]=>
  unicode(4) "test"
  [1001]=>
  unicode(6) "monkey"
  [16777216]=>
  float(-0.33333333333333)
  [17]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [u"test"]=>
  int(27)
}

 -- Testing usort() -- 
array(8) {
  [0]=>
  float(-0.33333333333333)
  [1]=>
  array(2) {
    [0]=>
    unicode(6) "banana"
    [1]=>
    unicode(6) "orange"
  }
  [2]=>
  int(27)
  [3]=>
  unicode(3) "PHP"
  [4]=>
  unicode(27) "PHP: Hypertext Preprocessor"
  [5]=>
  unicode(4) "Test"
  [6]=>
  unicode(6) "monkey"
  [7]=>
  unicode(4) "test"
}
