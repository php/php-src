--TEST--
Redeclare inherited private property as private static.
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
      private static $p = "B::p (static)";
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
