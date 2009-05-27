--TEST--
Returning a reference from a non-static method via another non-static method
--INI--
error_reporting = E_ALL & ~E_STRICT
--FILE--
<?php
class C {
	function returnConstantByValue() {
		return 100;
	}
	
	function &returnConstantByRef() {
		return 100;
	}
	
	function &returnVariableByRef() {
		return $GLOBALS['a'];
	}
	
	function &returnFunctionCallByRef($functionToCall) {
		return $this->$functionToCall();
	}
}
$c = new C;

echo "\n---> 1. Via a return by ref function call, assign by reference the return value of a function that returns by value:\n";
unset($a, $b);
$a = 4;
$b = &$c->returnFunctionCallByRef('returnConstantByValue');
$a++;
var_dump($a, $b);

echo "\n---> 2. Via a return by ref function call, assign by reference the return value of a function that returns a constant by ref:\n";
unset($a, $b);
$a = 4;
$b = &$c->returnFunctionCallByRef('returnConstantByRef');
$a++;
var_dump($a, $b);

echo "\n---> 3. Via a return by ref function call, assign by reference the return value of a function that returns by ref:\n";
unset($a, $b);
$a = 4;
$b = &$c->returnFunctionCallByRef('returnVariableByRef');
$a++;
var_dump($a, $b);

?>
--EXPECTF--

---> 1. Via a return by ref function call, assign by reference the return value of a function that returns by value:

Notice: Only variable references should be returned by reference in %s on line 16
int(5)
int(100)

---> 2. Via a return by ref function call, assign by reference the return value of a function that returns a constant by ref:

Notice: Only variable references should be returned by reference in %s on line 8
int(5)
int(100)

---> 3. Via a return by ref function call, assign by reference the return value of a function that returns by ref:
int(5)
int(5)