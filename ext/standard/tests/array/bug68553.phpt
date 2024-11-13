--TEST--
Bug #68553 (array_column: null values in $index_key become incrementing keys in result)
--FILE--
<?php
$i = 100;
/* increase the resource id to make test stable */
while ($i--) {
    $fd = fopen(__FILE__, "r");
    fclose($fd);
}
$a = [
    ['a' => 10],
    ['a' => 20],
    ['a' => true],
    ['a' => false],
    ['a' => fopen(__FILE__, "r")],
    ['a' => -5],
    ['a' => 7.38],
    ['a' => null, "test"],
    ['a' => null],
];

var_dump(array_column($a, null, 'a'));

try {
    var_dump(array_column([['a' => new stdClass]], null, 'a'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(array_column([['a' => []]], null, 'a'));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d

Deprecated: Implicit conversion from float 7.38 to int loses precision in %s on line %d
array(8) {
  [10]=>
  array(1) {
    ["a"]=>
    int(10)
  }
  [20]=>
  array(1) {
    ["a"]=>
    int(20)
  }
  [1]=>
  array(1) {
    ["a"]=>
    bool(true)
  }
  [0]=>
  array(1) {
    ["a"]=>
    bool(false)
  }
  [%d]=>
  array(1) {
    ["a"]=>
    resource(%d) of type (stream)
  }
  [-5]=>
  array(1) {
    ["a"]=>
    int(-5)
  }
  [7]=>
  array(1) {
    ["a"]=>
    float(7.38)
  }
  [""]=>
  array(1) {
    ["a"]=>
    NULL
  }
}
Cannot access offset of type stdClass on array
Cannot access offset of type array on array
