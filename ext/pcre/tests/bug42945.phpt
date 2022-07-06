--TEST--
Bug #42945 (preg_split() swallows part of the string)
--FILE--
<?php

var_dump(preg_match_all('/\b/', "a'", $m, PREG_OFFSET_CAPTURE));
var_dump($m);

var_dump(preg_split('/\b/', "a'"));
var_dump(preg_split('/\b/', "a'", -1, PREG_SPLIT_OFFSET_CAPTURE));
var_dump(preg_split('/\b/', "a'", -1, PREG_SPLIT_NO_EMPTY));
var_dump(preg_split('/\b/', "a'", -1, PREG_SPLIT_NO_EMPTY|PREG_SPLIT_OFFSET_CAPTURE));

?>
--EXPECT--
int(2)
array(1) {
  [0]=>
  array(2) {
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
      int(1)
    }
  }
}
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  string(1) "a"
  [2]=>
  string(1) "'"
}
array(3) {
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
    string(1) "'"
    [1]=>
    int(1)
  }
}
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "'"
}
array(2) {
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
    string(1) "'"
    [1]=>
    int(1)
  }
}
