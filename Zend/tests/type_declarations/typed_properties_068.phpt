--TEST--
Test typed static property by ref
--FILE--
<?php
function &ref($a = null) {
	static $f;
	if ($a !== null) $f = function &() use (&$a) { return $a; };
	return $f();
}

class Foo {
	public static int $i;
	public static string $s = "x";
}

Foo::$i = &ref(5);
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

Foo::$s = &ref(5);
var_dump(Foo::$s, ref());

Foo::$i = &ref("0");
var_dump(Foo::$i, ref());

try {
	Foo::$i = &ref("x");
} catch (TypeError $e) { print $e->getMessage()."\n"; }
var_dump(Foo::$i, ref());

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
Cannot assign null to reference held by property Foo::$i of type int
int(4)
int(4)
Typed property Foo::$i must be int, null used
int(4)
int(4)
string(1) "5"
string(1) "5"
int(0)
int(0)
Typed property Foo::$i must be int, string used
int(0)
string(1) "x"
Reference with value of type string held by property Foo::$s of type string is not compatible with property Foo::$i of type int
int(0)
string(1) "5"
Reference with value of type int held by property Foo::$i of type int is not compatible with property Foo::$s of type string
int(0)
string(1) "5"
