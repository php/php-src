--TEST--
Test typed static property by ref
--FILE--
<?php
function &intRef() {
	static $a = 5;
	$b = $a;
	$a = &$b;
	return $a;
}

function &stringRef() {
	static $a = "0";
	$b = $a;
	$a = &$b;
	return $a;
}

function &nonNumericStringRef() {
	static $a = "x";
	$b = $a;
	$a = &$b;
	return $a;
}

class Foo {
	public static int $i;
	public static string $s = "x";
}

Foo::$i = &intRef();
var_dump(Foo::$i);

$i = &Foo::$i;
$i = 2;
var_dump($i, Foo::$i);

$i = "3";
var_dump($i, Foo::$i);

Foo::$i = "4";
var_dump($i, Foo::$i);

try {
	$i = null;
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump($i, Foo::$i);

try {
	Foo::$i = null;
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump($i, Foo::$i);

Foo::$s = &intRef();
var_dump(Foo::$s, intRef());

Foo::$i = &stringRef();
var_dump(Foo::$i, stringRef());

try {
	Foo::$i = &nonNumericStringRef();
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump(Foo::$i, nonNumericStringRef());

try {
	Foo::$i = &Foo::$s;
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump(Foo::$i, Foo::$s);

try {
	Foo::$s = &Foo::$i;
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump(Foo::$i, Foo::$s);

?>
--EXPECT--
int(5)
int(2)
int(2)
int(3)
int(3)
int(4)
int(4)
Cannot assign null to reference of type int
int(4)
int(4)
Typed property Foo::$i must be int, null used
int(4)
int(4)
string(1) "5"
int(5)
int(0)
string(1) "0"
Cannot assign string to reference of type int
int(0)
string(1) "x"
Static property and reference types int and string are not compatible
int(0)
string(1) "5"
Static property and reference types string and int are not compatible
int(0)
string(1) "5"
