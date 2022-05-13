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

// objects are allowed in arrays
define('ELEPHPANT', [new StdClass]);
var_dump(ELEPHPANT);

// ensure recursion doesn't crash
$recursive = [];
$recursive[0] = &$recursive;

try {
    define('RECURSION', $recursive);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
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
array(1) {
  [0]=>
  object(stdClass)#1 (0) {
  }
}
define(): Argument #2 ($value) cannot be a recursive array
