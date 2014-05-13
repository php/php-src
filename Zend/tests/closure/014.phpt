--TEST--
Closure 014: return by value/reference
--FILE--
<?php
class C1 {
	function __invoke() {
		return 0;
	}
}
class C2 {
	function &__invoke(&$a) {
		return $a;
	}
}
class C3 {
	function __invoke() {
	}
}

$x = new C1();
var_dump($x());
var_dump($x->__invoke());
$x();
$x->__invoke();
$x = function() {
	return 0;
};
var_dump($x());
var_dump($x->__invoke());
$x();
$x->__invoke();

$x = new C2();
$a = $b = $c = $d = 1;
$e =& $x($a);
$e = 2;
var_dump($a);
$e =& $x->__invoke($b);
$e = 3;
var_dump($b);
$x($b);
$x->__invoke($b);
$x = function & (&$a) {
	return $a;
};
$e =& $x($c);
$e = 4;
var_dump($c);
$e =& $x->__invoke($d);
$e = 5;
var_dump($d);
$x($d);
$x->__invoke($d);

$x = new C3();
var_dump($x());
var_dump($x->__invoke());
$x();
$x->__invoke();
$x = function() {
};
var_dump($x());
var_dump($x->__invoke());
$x();
$x->__invoke();
?>
--EXPECT--
int(0)
int(0)
int(0)
int(0)
int(2)
int(3)
int(4)
int(5)
NULL
NULL
NULL
NULL
