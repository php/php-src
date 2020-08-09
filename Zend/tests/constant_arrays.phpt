--TEST--
Constant arrays
--INI--
zend.enable_gc=1
--FILE--
<?php

define('FOOBAR', [1, 2, 3, ['foo' => 'bar']]);
const FOO_BAR = [1, 2, 3, ['foo' => 'bar']];

$x = FOOBAR;
$x[0] = 7;
var_dump($x, FOOBAR);

$x = FOO_BAR;
$x[0] = 7;
var_dump($x, FOO_BAR);

// ensure references are removed
$x = 7;
$y = [&$x];
define('QUX', $y);
$y[0] = 3;
var_dump($x, $y, QUX);

// ensure objects not allowed in arrays
try {
    define('ELEPHPANT', [new StdClass]);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

// ensure recursion doesn't crash
$recursive = [];
$recursive[0] = &$recursive;

try {
    define('RECURSION', $recursive);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECTF--
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
int(3)
array(1) {
  [0]=>
  &int(3)
}
array(1) {
  [0]=>
  int(7)
}
define(): Argument #2 ($value) cannot be an object, stdClass given
define(): Argument #2 ($value) cannot be a recursive array
