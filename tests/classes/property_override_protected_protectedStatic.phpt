--TEST--
Redeclare inherited protected property as protected static.
--FILE--
<?php
  class A
  {
      protected $p = "A::p";
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

Fatal error: Cannot redeclare non static A::$p as static B::$p in %s on line 18
