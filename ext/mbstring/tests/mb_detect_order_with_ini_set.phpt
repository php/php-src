--TEST--
Test mb_detect_order() function : ini set changes order
--EXTENSIONS--
mbstring
--FILE--
<?php

ini_set('mbstring.detect_order', 'UTF-8, ISO-8859-1, ASCII');

var_dump( mb_detect_order());

ini_set('mbstring.detect_order', 'UTF-8');

var_dump( mb_detect_order());

?>
--EXPECT--
array(3) {
  [0]=>
  string(5) "UTF-8"
  [1]=>
  string(10) "ISO-8859-1"
  [2]=>
  string(5) "ASCII"
}
array(1) {
  [0]=>
  string(5) "UTF-8"
}
