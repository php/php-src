--TEST--
Redeclare inherited public static property as protected static.
--FILE--
<?php
  class A
  {
      public static $p = "A::p (static)";
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
Fatal error: Property B::$p must have public visibility to be compatible with overridden property A::$p in %s on line %d
