--TEST--
Redeclare inherited protected static property as public static.
--FILE--
<?php
  class A
  {
      protected static $p = "A::p (static)";
      static function showA()
      {
          echo self::$p . "\n";
      }
  }
  
  class B extends A
  {
      public static $p = "B::p (static)";
      static function showB()
      {
          echo self::$p . "\n";
      }
  }
  
  
  A::showA();
  
  B::showA();
  B::showB();
?>
--EXPECTF--

Fatal error: Cannot change initial value of property static protected A::$p in class B in %s on line 18
