--TEST--
Test hash packed fill number of elements
--EXTENSIONS--
zend_test
--FILE--
<?php

function number() {
    return 6;
}

$my_array = [number() => 0];
zend_test_fill_packed_array($my_array);

var_dump($my_array);
var_dump(count($my_array));

?>
--EXPECT--
array(11) {
  [6]=>
  int(0)
  [7]=>
  int(0)
  [8]=>
  int(1)
  [9]=>
  int(2)
  [10]=>
  int(3)
  [11]=>
  int(4)
  [12]=>
  int(5)
  [13]=>
  int(6)
  [14]=>
  int(7)
  [15]=>
  int(8)
  [16]=>
  int(9)
}
int(11)
