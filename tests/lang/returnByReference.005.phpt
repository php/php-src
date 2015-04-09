--TEST--
Returning a reference from a method
--FILE--
<?php
Class C {
	function returnConstantByValue() {
		return 100;
	}
	
	function &returnConstantByRef() {
		return 100;
	}
	
	static function &returnVariableByRef() {
		return $GLOBALS['a'];
	}
}
$c = new C;

echo "\n---> 1. Trying to assign by reference the return value of a function that returns by value:\n";
unset($a, $b);
$a = 4;
$b = &$c->returnConstantByValue();
$a++;
var_dump($a, $b);

echo "\n---> 2. Trying to assign by reference the return value of a function that returns a constant by ref:\n";
unset($a, $b);
$a = 4;
$b = &$c->returnConstantByRef();
$a++;
var_dump($a, $b);

echo "\n---> 3. Trying to assign by reference the return value of a function that returns by ref:\n";
unset($a, $b);
$a = 4;
$b = &$c->returnVariableByRef();
$a++;
var_dump($a, $b);

?>
--EXPECTF--

---> 1. Trying to assign by reference the return value of a function that returns by value:

Strict Standards: Only variables should be assigned by reference in %s on line 20
int(5)
int(100)

---> 2. Trying to assign by reference the return value of a function that returns a constant by ref:

Notice: Only variable references should be returned by reference in %s on line 8
int(5)
int(100)

---> 3. Trying to assign by reference the return value of a function that returns by ref:
int(5)
int(5)
