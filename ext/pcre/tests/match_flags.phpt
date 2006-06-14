--TEST--
preg_match_all() flags
--FILE--
<?php

var_dump(preg_match_all('/(.)x/', 'zxax', $match, PREG_PATTERN_ORDER));
var_dump($match);

var_dump(preg_match_all('/(.)x/', 'zxyx', $match, PREG_SET_ORDER));
var_dump($match);

var_dump(preg_match_all('/(.)x/', 'zxyx', $match, PREG_OFFSET_CAPTURE));
var_dump($match);

var_dump(preg_match_all('/(.)x/', 'zxyx', $match, PREG_SET_ORDER | PREG_OFFSET_CAPTURE));
var_dump($match);

?>
--EXPECT--
int(2)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "zx"
    [1]=>
    string(2) "ax"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "z"
    [1]=>
    string(1) "a"
  }
}
int(2)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "zx"
    [1]=>
    string(1) "z"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "yx"
    [1]=>
    string(1) "y"
  }
}
int(2)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(2) "zx"
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      string(2) "yx"
      [1]=>
      int(2)
    }
  }
  [1]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(1) "z"
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "y"
      [1]=>
      int(2)
    }
  }
}
int(2)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(2) "zx"
      [1]=>
      int(0)
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "z"
      [1]=>
      int(0)
    }
  }
  [1]=>
  array(2) {
    [0]=>
    array(2) {
      [0]=>
      string(2) "yx"
      [1]=>
      int(2)
    }
    [1]=>
    array(2) {
      [0]=>
      string(1) "y"
      [1]=>
      int(2)
    }
  }
}
