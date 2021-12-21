--TEST--
Test array_unique() with ARRAY_UNIQUE_IDENTICAL
--FILE--
<?php

var_dump(array_unique(['1234', '1234'], ARRAY_UNIQUE_IDENTICAL));
var_dump(array_unique(['1234', 1234, 1234.0, '1234', 1234, 1234.0], ARRAY_UNIQUE_IDENTICAL));
var_dump(array_unique([0, '0', 0.0, '0.0', '', null, null], ARRAY_UNIQUE_IDENTICAL));

$a = (object)[];
$b = (object)[];
$a2 = [$a];
$b2 = [$b];
$a3 = (object)['foo' => $a];
$b3 = (object)['foo' => $b];
var_dump(array_unique([$a, $b, $a2, $b2, $a3, $b3], ARRAY_UNIQUE_IDENTICAL));

?>
--EXPECT--
array(1) {
  [0]=>
  string(4) "1234"
}
array(3) {
  [0]=>
  string(4) "1234"
  [1]=>
  int(1234)
  [2]=>
  float(1234)
}
array(6) {
  [0]=>
  int(0)
  [1]=>
  string(1) "0"
  [2]=>
  float(0)
  [3]=>
  string(3) "0.0"
  [4]=>
  string(0) ""
  [5]=>
  NULL
}
array(6) {
  [0]=>
  object(stdClass)#1 (0) {
  }
  [1]=>
  object(stdClass)#2 (0) {
  }
  [2]=>
  array(1) {
    [0]=>
    object(stdClass)#1 (0) {
    }
  }
  [3]=>
  array(1) {
    [0]=>
    object(stdClass)#2 (0) {
    }
  }
  [4]=>
  object(stdClass)#3 (1) {
    ["foo"]=>
    object(stdClass)#1 (0) {
    }
  }
  [5]=>
  object(stdClass)#4 (1) {
    ["foo"]=>
    object(stdClass)#2 (0) {
    }
  }
}
