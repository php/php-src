--TEST--
Constant arrays
--FILE--
<?php

define('FOOBAR', [1, 2, 3, ['foo' => 'bar']]);
const FOO_BAR = [1, 2, 3, ['foo' => 'bar']];

var_dump(FOOBAR);
var_dump(FOO_BAR);

$x = FOOBAR;
$x[0] = 7;
var_dump($x, FOOBAR);

$x = FOO_BAR;
$x[0] = 7;
var_dump($x, FOO_BAR);
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}
array(4) {
  [0]=>
  int(7)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}
array(4) {
  [0]=>
  int(7)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  array(1) {
    ["foo"]=>
    string(3) "bar"
  }
}