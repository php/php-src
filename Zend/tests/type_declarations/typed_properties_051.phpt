--TEST--
Weak casts must not leak
--FILE--
<?php

class A {
	public string $a;
}
class B {
	function __toString() {
		return str_repeat("ok", 2);
	}
}
class C {
}
$o = new A;
$o->a = new B;
var_dump($o->a);
try {
	$o->a = new C;
} catch (Throwable $e) {
	echo $e->getMessage()."\n";
}
?>
--EXPECT--
string(4) "okok"
Typed property A::$a must be string, C used
