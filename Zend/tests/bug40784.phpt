--TEST--
Bug #40784 (Case sensivity in constructor's fallback)
--FILE--
<?php

class A {
	  function A () { echo "I'm A\n"; }
}

class B extends A {
  function __construct() {
    parent::__construct();
    parent::__constrUct();
  }
}

$b = new B;

echo "Done\n";
?>
--EXPECTF--	
I'm A
I'm A
Done
