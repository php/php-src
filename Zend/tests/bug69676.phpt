--TEST--
Bug #69676: Resolution of self::FOO in class constants not correct
--FILE--
<?php
class A {
	const myConst = "const in A";
	const myDynConst = self::myConst;
}

class B extends A {
	const myConst = "const in B";
}

var_dump(B::myDynConst);
var_dump(A::myDynConst);
?>
--EXPECT--
string(10) "const in A"
string(10) "const in A"
