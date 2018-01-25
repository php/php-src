--TEST--
Redeclare inherited protected static property as protected static.
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
      protected static $p = "B::p (static)";
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
A::p (static)
A::p (static)
B::p (static)
