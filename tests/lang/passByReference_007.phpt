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
try {
	foo(returnVal());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($a);

echo "Pass a function call that returns a reference:\n";
$a = "original";
try {
	foo(returnReference());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($a);


echo "\nPass a static method call that returns a value:\n";
$a = "original";
try {
	foo(C::sreturnVal());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($a);

echo "Pass a static method call that returns a reference:\n";
$a = "original";
try{
	foo(C::sreturnReference());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($a);


$myC = new C;
echo "\nPass a method call that returns a value:\n";
$a = "original";
try {
	foo($myC->returnVal());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($a);

echo "Pass a method call that returns a reference:\n";
$a = "original";
try {
	foo($myC->returnReference());
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
var_dump($a);

?>
--EXPECT--
Pass a function call that returns a value:
Exception: Cannot pass parameter 1 by reference
string(8) "original"
Pass a function call that returns a reference:
string(8) "original"
string(7) "changed"

Pass a static method call that returns a value:
Exception: Cannot pass parameter 1 by reference
string(8) "original"
Pass a static method call that returns a reference:
string(8) "original"
string(7) "changed"

Pass a method call that returns a value:
Exception: Cannot pass parameter 1 by reference
string(8) "original"
Pass a method call that returns a reference:
string(8) "original"
string(7) "changed"
