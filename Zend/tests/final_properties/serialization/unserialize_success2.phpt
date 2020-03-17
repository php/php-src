--TEST--
Test that final properties can be unserialized even when they were serialized as non-final
--FILE--
<?php

class MyClass
{
    final public int $finalProp = 1;
}

var_dump(unserialize('O:7:"MyClass":2:{s:9:"finalProp";i:2;s:7:"refProp";R:2;}'));

?>
--EXPECT--
object(MyClass)#1 (2) {
  ["finalProp"]=>
  &int(2)
  ["refProp"]=>
  &int(2)
}
