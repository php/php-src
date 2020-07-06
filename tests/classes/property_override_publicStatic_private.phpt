--TEST--
Redeclare inherited public static property as private.
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
      private $p = "B::p";
      function showB()
      {
          echo $this->p . "\n";
      }
  }


  A::showA();

  $b = new B;
  $b->showA();
  $b->showB();
?>
--EXPECTF--
Fatal error: Property B::$p must be static to be compatible with overridden property A::$p in %s on line %d
