--TEST--
Accept variadic arguments by reference
--FILE--
<?php

use extension string StringHandler;

class StringHandler {
    public static function incrementArgs($self, &...$args) {
        foreach ($args as &$arg) $arg++;
    }
}

$string = "foo";
$array = [0, 1, 2];
$string->incrementArgs($array[0], $array[1], $array[2]);
var_dump($array);

?>
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
