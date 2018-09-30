--TEST--
Modification of typed property during assignment must not leak
--FILE--
<?php

class A {
	public string $prop = "";
}

class B {
	public function __toString() {
		global $a;
		$a->prop = "dont ";
		$a->prop .= "leak ";
		$a->prop .= "me!";
		return "test";
	}
}

$a = new A;
$a->prop = new B;
var_dump($a);

$a = new A;
$prop = &$a->prop;
$a->prop = new B;
var_dump($a);

?>
--EXPECTF--
object(A)#1 (1) {
  ["prop"]=>
  string(4) "test"
}
object(A)#%d (1) {
  ["prop"]=>
  &string(4) "test"
}
