--TEST--
Bug #29446 (ZE allows multiple declarations of the same class constant)
--FILE--
<?php

class testClass {
  const TEST_CONST = 'test';
  const TEST_CONST = 'test1';
  
  function testClass() {
    echo self::TEST_CONST;
  }
}

$test = new testClass;

?>
--EXPECTF--
Fatal error: Cannot redefine class constant testClass::TEST_CONST in %s on line 5