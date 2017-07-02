--TEST--
Bug#71412 ArrayIterator reflection parameter info
--SKIPIF--
<?php
extension_loaded('SPL') || print "skip";
--FILE--
<?php
echo (new ReflectionMethod('ArrayIterator', '__construct'));
--EXPECT--
Method [ <internal:SPL, ctor> public method __construct ] {

  - Parameters [2] {
    Parameter #0 [ <optional> $array ]
    Parameter #1 [ <optional> $ar_flags ]
  }
}
