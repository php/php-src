--TEST--
Pass function and method calls by reference and by value.
--FILE--
<?php
class C {
	static function sreturnVal() {
		global $a;
		return $a;
	}

	static function &sreturnReference() {
		global $a;
		return $a;
	}

	function returnVal() {
		global $a;
		return $a;
	}

	function &returnReference() {
		global $a;
		return $a;
	}
}

function returnVal() {
		global $a;
		return $a;
}

function &returnReference() {
		global $a;
		return $a;
}



function foo(&$ref) {
	var_dump($ref);
	$ref = "changed";
}


echo "Pass a function call that returns a value:\n";
$a = "original";
foo(returnVal());
var_dump($a);

echo "Pass a function call that returns a reference:\n";
$a = "original";
foo(returnReference());
var_dump($a);


echo "\nPass a static method call that returns a value:\n";
$a = "original";
foo(C::sreturnVal());
var_dump($a);

echo "Pass a static method call that returns a reference:\n";
$a = "original";
foo(C::sreturnReference());
var_dump($a);


$myC = new C;
echo "\nPass a method call that returns a value:\n";
$a = "original";
foo($myC->returnVal());
var_dump($a);

echo "Pass a method call that returns a reference:\n";
$a = "original";
foo($myC->returnReference());
var_dump($a);

?>
--EXPECTF--
Pass a function call that returns a value:

Notice: Only variables should be passed by reference in %s on line 44
string(8) "original"
string(8) "original"
Pass a function call that returns a reference:
string(8) "original"
string(7) "changed"

Pass a static method call that returns a value:

Notice: Only variables should be passed by reference in %s on line 55
string(8) "original"
string(8) "original"
Pass a static method call that returns a reference:
string(8) "original"
string(7) "changed"

Pass a method call that returns a value:

Notice: Only variables should be passed by reference in %s on line 67
string(8) "original"
string(8) "original"
Pass a method call that returns a reference:
string(8) "original"
string(7) "changed"
