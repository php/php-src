--TEST--
preg_match() flags
--FILE--
<?php

var_dump(preg_match('/x(.)/', 'fjszxax', $match, PREG_OFFSET_CAPTURE));
var_dump($match);

var_dump(preg_match('/(.)x/', 'fjszxax', $match, PREG_OFFSET_CAPTURE, 4));
var_dump($match);

var_dump(preg_match('/(?P<capt1>.)(x)(?P<letsmix>\S+)/', 'fjszxax', $match, PREG_OFFSET_CAPTURE));
var_dump($match);

?>
--EXPECT--
int(1)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "xa"
    [1]=>
    int(4)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(5)
  }
}
int(1)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "ax"
    [1]=>
    int(5)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(5)
  }
}
int(1)
array(6) {
  [0]=>
  array(2) {
    [0]=>
    string(4) "zxax"
    [1]=>
    int(3)
  }
  ["capt1"]=>
  array(2) {
    [0]=>
    string(1) "z"
    [1]=>
    int(3)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "z"
    [1]=>
    int(3)
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "x"
    [1]=>
    int(4)
  }
  ["letsmix"]=>
  array(2) {
    [0]=>
    string(2) "ax"
    [1]=>
    int(5)
  }
  [3]=>
  array(2) {
    [0]=>
    string(2) "ax"
    [1]=>
    int(5)
  }
}
