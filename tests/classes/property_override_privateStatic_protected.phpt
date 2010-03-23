--TEST--
Redeclare inherited private static property as protected.
--FILE--
<?php
  class A
  {
      private static $p = "A::p (static)";
      static function showA()
      {
          echo self::$p . "\n";
      }
  }
  
  class B extends A
  {
      protected $p = "B::p";
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
A::p (static)
A::p (static)
B::p
