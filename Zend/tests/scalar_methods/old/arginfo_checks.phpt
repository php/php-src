--TEST--
Arginfo checks for argument passed to handler methods
--FILE--
<?php

use extension int IntHandler;
use extension array ArrayHandler;

class IntHandler {
    public static function pushOnto($self, array &$arr) {
        $arr[] = $self;
    }
}

class ArrayHandler {
    public static function reverse(array $self) {
        return array_reverse($self);
    }
}

$arr = [1, 2, 3];
$arr = $arr->reverse();
var_dump($arr);

$zero = 0;
$zero->pushOnto($arr);
var_dump($arr);

?>
--EXPECT--
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
array(4) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
  [3]=>
  int(0)
}
