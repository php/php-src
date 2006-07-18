--TEST--
preg_split() 2nd test
--FILE--
<?php

var_dump(preg_split('/(\d*)/', 'ab2c3u', -1, PREG_SPLIT_DELIM_CAPTURE));
var_dump(preg_split('/(\d*)/', 'ab2c3u', -1, PREG_SPLIT_OFFSET_CAPTURE));
var_dump(preg_split('/(\d*)/', 'ab2c3u', -1, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE));
var_dump(preg_split('/(\d*)/', 'ab2c3u', -1, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_OFFSET_CAPTURE));;
var_dump(preg_split('/(\d*)/', 'ab2c3u', -1, PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_OFFSET_CAPTURE));
var_dump(preg_split('/(\d*)/', 'ab2c3u', -1, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_OFFSET_CAPTURE));


var_dump(preg_last_error(1));
ini_set('pcre.recursion_limit', 1);
var_dump(preg_last_error() == PREG_NO_ERROR);
var_dump(preg_split('/(\d*)/', 'ab2c3u'));
var_dump(preg_last_error() == PREG_RECURSION_LIMIT_ERROR);

?>
--EXPECTF--
array(15) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(1) "a"
  [3]=>
  string(0) ""
  [4]=>
  string(1) "b"
  [5]=>
  string(1) "2"
  [6]=>
  string(0) ""
  [7]=>
  string(0) ""
  [8]=>
  string(1) "c"
  [9]=>
  string(1) "3"
  [10]=>
  string(0) ""
  [11]=>
  string(0) ""
  [12]=>
  string(1) "u"
  [13]=>
  string(0) ""
  [14]=>
  string(0) ""
}
array(8) {
  [0]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(0)
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    int(1)
  }
  [3]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(3)
  }
  [4]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    int(3)
  }
  [5]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(5)
  }
  [6]=>
  array(2) {
    [0]=>
    string(1) "u"
    [1]=>
    int(5)
  }
  [7]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(6)
  }
}
array(6) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "2"
  [3]=>
  string(1) "c"
  [4]=>
  string(1) "3"
  [5]=>
  string(1) "u"
}
array(4) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    int(1)
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    int(3)
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "u"
    [1]=>
    int(5)
  }
}
array(15) {
  [0]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(0)
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(0)
  }
  [3]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(1)
  }
  [4]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    int(1)
  }
  [5]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    int(2)
  }
  [6]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(3)
  }
  [7]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(3)
  }
  [8]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    int(3)
  }
  [9]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    int(4)
  }
  [10]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(5)
  }
  [11]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(5)
  }
  [12]=>
  array(2) {
    [0]=>
    string(1) "u"
    [1]=>
    int(5)
  }
  [13]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(6)
  }
  [14]=>
  array(2) {
    [0]=>
    string(0) ""
    [1]=>
    int(6)
  }
}
array(6) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    int(1)
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    int(2)
  }
  [3]=>
  array(2) {
    [0]=>
    string(1) "c"
    [1]=>
    int(3)
  }
  [4]=>
  array(2) {
    [0]=>
    string(1) "3"
    [1]=>
    int(4)
  }
  [5]=>
  array(2) {
    [0]=>
    string(1) "u"
    [1]=>
    int(5)
  }
}

Warning: preg_last_error() expects exactly 0 parameters, 1 given in %s on line %d
NULL
bool(true)
array(1) {
  [0]=>
  string(6) "ab2c3u"
}
bool(true)
