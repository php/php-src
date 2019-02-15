--TEST--
Redeclare inherited private property as protected.
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
--EXPECT--
A::p
A::p
B::p
