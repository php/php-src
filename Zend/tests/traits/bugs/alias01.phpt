--TEST--
Aliases are applied to the correct methods, and only to them.
--FILE--
<?php
trait T1 {
  function m1() { echo "T:m1\n"; }
  function m2() { echo "T:m2\n"; }
}

class C1 { 
  use T1 { m1 as a1; }
}

$o = new C1;
$o->m1();
$o->a1();
$o->m2();
$o->a2();

?>
--EXPECTF--
T:m1
T:m1
T:m2

Fatal error: Call to undefined method C1::a2() in %s on line %d
