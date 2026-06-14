--TEST--
Test mb_detect_order() function : ini set changes order
--EXTENSIONS--
mbstring
--INI--
mbstring.detect_order=UTF-8,ISO-8859-15,ISO-8859-1,ASCII
--FILE--
<?php

var_dump( mb_detect_order());

ini_set('mbstring.detect_order', 'UTF-8, ISO-8859-1, ASCII');

var_dump( mb_detect_order());

ini_set('mbstring.detect_order', 'UTF-8');

var_dump( mb_detect_order());

ini_set('mbstring.detect_order', NULL);

var_dump( mb_detect_order());

ini_set('mbstring.detect_order', '');

var_dump( mb_detect_order());

?>
--EXPECT--
array(4) {
  [0]=>
  string(5) "UTF-8"
  [1]=>
  string(11) "ISO-8859-15"
  [2]=>
  string(10) "ISO-8859-1"
  [3]=>
  string(5) "ASCII"
}
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
array(1) {
  [0]=>
  string(5) "UTF-8"
}
array(1) {
  [0]=>
  string(5) "UTF-8"
}

