--TEST--
Redeclare inherited protected property as private (duplicates Zend/tests/errmsg_023.phpt).
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
      private $p = "B::p";
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
Fatal error: Property B::$p must have protected or public visibility to be compatible with overridden property A::$p in %s on line %d
