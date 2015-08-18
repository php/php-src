--TEST--
Private property inheritance check
--FILE--
<?php
Class A {
	private $c;
}

Class B extends A {
	private $c;
}

Class C extends B {
}

var_dump(new C);
?>
--EXPECTF--
object(C)#%d (2) {
  [%u|b%"c":%u|b%"B":private]=>
  NULL
  [%u|b%"c":%u|b%"A":private]=>
  NULL
}
