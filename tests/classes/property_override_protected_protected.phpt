--TEST--
Redeclare inherited protected property as protected.
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
      protected $p = "B::p";
      function showB()
      {
          echo $this->p . "\n";
      }
  }
  
  
  $a = new A;
  $a->showA();
  
  $b = new B;
  $b->showA();
  $b->showB();
?>
--EXPECTF--
A::p
B::p
B::p
