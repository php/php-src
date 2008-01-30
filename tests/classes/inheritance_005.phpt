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
  
  
  echo "About to construct new B: ";
  $b = new B;
  echo "About to invoke implicit B::B(): ";
  $b->B();
  
  echo "\nAbout to construct new C: ";
  $c = new C;
  echo "About to invoke implicit C::B(): ";
  $c->B();
  echo "About to invoke implicit C::C(): ";
  $c->C();
?>
--EXPECTF--
About to construct new B: In A::A
About to invoke implicit B::B(): In A::A

About to construct new C: In A::A
About to invoke implicit C::B(): In A::A
About to invoke implicit C::C(): In A::A


