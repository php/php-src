--TEST--
Test typed static property
--FILE--
<?php
function &ref() {
	static $a = 5;
	return $a;
}

class Foo {
	public static int $i;
	public static string $s = "x";
}

var_dump(Foo::$s);
Foo::$i = 1;
var_dump(Foo::$i);
Foo::$i = "1";
var_dump(Foo::$i);
Foo::$s = Foo::$i++;
var_dump(Foo::$s, Foo::$i);
$a = 3;
Foo::$s = $a;
var_dump(Foo::$s);
Foo::$i = "4";
var_dump(Foo::$i);

Foo::$i = ref();
var_dump(Foo::$i);
Foo::$s = ref();
var_dump(Foo::$s);
var_dump(ref());
?>
--EXPECT--
string(1) "x"
int(1)
int(1)
string(1) "1"
int(2)
string(1) "3"
int(4)
int(5)
string(1) "5"
int(5)
