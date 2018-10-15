--TEST--
Test constants with default values based on other constants.
--FILE--
<?php
  class C
  {
      const CONST_2 = self::CONST_1;
      const CONST_1 = self::BASE_CONST;
      const BASE_CONST = 'hello';
  }
  var_dump(C::CONST_1, C::CONST_2);
?>
--EXPECT--
string(5) "hello"
string(5) "hello"
