--TEST--
ReflectionClass::export() - array constants
--FILE--
<?php
Class A {
	const A = 8;
	const B = ["a", "b"];
}
ReflectionClass::export("A");
?>
--EXPECTF--
Class [ <user> class A ] {
  @@ %s 2-5

  - Constants [2] {
    Constant [ public int A ] { 8 }
    Constant [ public array B ] { Array }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
