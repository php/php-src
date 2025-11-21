--TEST--
private(namespace) method with array_map - same namespace
--FILE--
<?php

namespace Foo;

class A {
    private(namespace) static function double($x) {
        return $x * 2;
    }
}

// Same namespace - should work
$result = array_map([A::class, 'double'], [1, 2, 3]);
var_dump($result);

?>
--EXPECT--
array(3) {
  [0]=>
  int(2)
  [1]=>
  int(4)
  [2]=>
  int(6)
}
