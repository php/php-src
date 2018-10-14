--TEST--
Redeclare inherited private property as protected static.
--FILE--
<?php
  class A
  {
      private $p = "A::p";
      function showA()
      {
          echo $this->p . "\n";
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


  $a = new A;
  $a->showA();

  $b = new B;
  $b->showA();
  B::showB();
?>
--EXPECTF--
A::p
A::p
B::p (static)
