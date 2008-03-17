--TEST--
Check for inherited old-style constructor.
--FILE--
<?php
  class A
  {
      function A()
      {
          echo "In " . __METHOD__ . "\n";
      }
  }
  
  class B extends A
  {
  }
  
  class C extends B
  {
  }
  
  
  echo "About to construct new B: \n";
  $b = new B;
  
  echo "Is B::B() callable?\n";
  var_dump(is_callable(array($b, "B")));
  
  echo "Is B::A() callable?\n";
  var_dump(is_callable(array($b, "A")));
  
  echo "About to construct new C: \n";
  $c = new C;

  echo "Is C::A() callable?\n";
  var_dump(is_callable(array($c, "A")));

  echo "Is C::B() callable?\n";
  var_dump(is_callable(array($c, "B")));

  echo "Is C::C() callable?\n";
  var_dump(is_callable(array($c, "C")));
?>
--EXPECTF--
About to construct new B: 
In A::A
Is B::B() callable?
bool(false)
Is B::A() callable?
bool(true)
About to construct new C: 
In A::A
Is C::A() callable?
bool(true)
Is C::B() callable?
bool(false)
Is C::C() callable?
bool(false)
