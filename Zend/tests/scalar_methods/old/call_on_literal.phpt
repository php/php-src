--TEST--
Calling methods on literals
--SKIPIF--
<?php if (PHP_VERSION_ID < 70000) die("skip Only supported on PHP 7");
--FILE--
<?php

use extension int IntHandler;
use extension string StringHandler;
use extension array ArrayHandler;

class IntHandler {
    public static function reverse($self) {
        return (int) strrev((string) $self);
    }
}
class StringHandler {
    public static function reverse($self) {
        return strrev($self);
    }
}
class ArrayHandler {
    public static function reverse($self) {
        return array_reverse($self);
    }
}

var_dump((123)->reverse());
var_dump("123"->reverse());
var_dump([1, 2, 3]->reverse());

?>
--EXPECT--
int(321)
string(3) "321"
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
