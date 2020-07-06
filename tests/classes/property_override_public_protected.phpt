--TEST--
Redeclare inherited public property as protected.
--FILE--
<?php
  class A
  {
      public $p = "A::p";
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
Fatal error: Property B::$p must have public visibility to be compatible with overridden property A::$p in %s on line %d
