--TEST--
Bug#71412 ArrayIterator reflection parameter info
--FILE--
<?php
echo (new ReflectionMethod('ArrayIterator', '__construct'));
?>
--EXPECT--
Method [ <internal:SPL, ctor> public method __construct ] {

  - Parameters [2] {
    Parameter #0 [ <optional> object|array $array = [] ]
    Parameter #1 [ <optional> int $flags = 0 ]
  }
}
