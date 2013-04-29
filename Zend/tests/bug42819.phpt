--TEST--
Bug #42819 (namespaces in indexes of constant arrays)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip SPL extension required"; ?>
--FILE--
<?php
namespace foo\foo;

const C = "foo\\foo\\C\n";
const I = 12;

class foo {
const I = 32;
const C = "foo\\foo\\foo::C\n";
}

namespace foo;
use \ArrayObject;

const C = "foo\\C\n";
const I = 11;

class foo {
	const C = "foo\\foo::C\n";
	const I = 22;
	const C1 = C;
	const C2 = foo\C;
	const C3 = foo\foo::C;
	const C4 = \foo\C;
	const C5 = \foo\foo::C;
	const C6 = ArrayObject::STD_PROP_LIST;
	const C7 = E_ERROR;
}

class bar1 {
	static $a1 = array(I => 0);
	static $a2 = array(foo\I => 0);
	static $a3 = array(foo\foo::I => 0);
	static $a4 = array(\foo\I => 0);
	static $a5 = array(\foo\foo::I => 0);
	static $a6 = array(ArrayObject::STD_PROP_LIST => 0);
	static $a7 = array(E_ERROR => 0);
}

class bar2 {
	static $a1 = array(I => I);
	static $a2 = array(foo\I => I);
	static $a3 = array(foo\foo::I => I);
	static $a4 = array(\foo\I => I);
	static $a5 = array(\foo\foo::I => I);
	static $a6 = array(ArrayObject::STD_PROP_LIST => I);
	static $a7 = array(E_ERROR => I);
}

class bar3 {
	static $a1 = array(I => foo\I);
	static $a2 = array(foo\I => foo\I);
	static $a3 = array(foo\foo::I => foo\I);
	static $a4 = array(\foo\I => foo\I);
	static $a5 = array(\foo\foo::I => foo\I);
	static $a6 = array(ArrayObject::STD_PROP_LIST => foo\I);
	static $a7 = array(E_ERROR => foo\I);
}

class bar4 {
	static $a1 = array(I => ArrayObject::STD_PROP_LIST);
	static $a2 = array(foo\I => ArrayObject::STD_PROP_LIST);
	static $a3 = array(foo\foo::I => ArrayObject::STD_PROP_LIST);
	static $a4 = array(\foo\I => ArrayObject::STD_PROP_LIST);
	static $a5 = array(\foo\foo::I => ArrayObject::STD_PROP_LIST);
	static $a6 = array(ArrayObject::STD_PROP_LIST => ArrayObject::STD_PROP_LIST);
	static $a7 = array(E_ERROR => ArrayObject::STD_PROP_LIST);
}

class bar5 {
	static $a1 = array(I => E_ERROR);
	static $a2 = array(foo\I => E_ERROR);
	static $a3 = array(foo\foo::I => E_ERROR);
	static $a4 = array(\foo\I => E_ERROR);
	static $a5 = array(\foo\foo::I => E_ERROR);
	static $a6 = array(ArrayObject::STD_PROP_LIST => E_ERROR);
	static $a7 = array(E_ERROR => E_ERROR);
}

echo "first\n";
echo C;
echo foo\C;
echo foo\foo::C;
echo foo::C;
echo \foo\foo::C;
echo ArrayObject::STD_PROP_LIST . "\n";
echo E_ERROR . "\n";
echo "second\n";
echo \foo\foo::C1;
echo \foo\foo::C2;
echo \foo\foo::C3;
echo \foo\foo::C4;
echo \foo\foo::C5;
echo \foo\foo::C6 . "\n";
echo \foo\foo::C7 . "\n";

print_r(bar1::$a1);
print_r(bar1::$a2);
print_r(bar1::$a3);
print_r(bar1::$a4);
print_r(bar1::$a5);
print_r(bar1::$a6);
print_r(bar1::$a7);

print_r(bar2::$a1);
print_r(bar2::$a2);
print_r(bar2::$a3);
print_r(bar2::$a4);
print_r(bar2::$a5);
print_r(bar2::$a6);
print_r(bar2::$a7);

print_r(bar3::$a1);
print_r(bar3::$a2);
print_r(bar3::$a3);
print_r(bar3::$a4);
print_r(bar3::$a5);
print_r(bar3::$a6);
print_r(bar3::$a7);

print_r(bar4::$a1);
print_r(bar4::$a2);
print_r(bar4::$a3);
print_r(bar4::$a4);
print_r(bar4::$a5);
print_r(bar4::$a6);
print_r(bar4::$a7);

print_r(bar5::$a1);
print_r(bar5::$a2);
print_r(bar5::$a3);
print_r(bar5::$a4);
print_r(bar5::$a5);
print_r(bar5::$a6);
print_r(bar5::$a7);
function oops($a = array(foo\unknown)){}
oops();
?>
--EXPECTF--
first
foo\C
foo\foo\C
foo\foo\foo::C
foo\foo::C
foo\foo::C
1
1
second
foo\C
foo\foo\C
foo\foo\foo::C
foo\C
foo\foo::C
1
1
Array
(
    [11] => 0
)
Array
(
    [12] => 0
)
Array
(
    [32] => 0
)
Array
(
    [11] => 0
)
Array
(
    [22] => 0
)
Array
(
    [1] => 0
)
Array
(
    [1] => 0
)
Array
(
    [11] => 11
)
Array
(
    [12] => 11
)
Array
(
    [32] => 11
)
Array
(
    [11] => 11
)
Array
(
    [22] => 11
)
Array
(
    [1] => 11
)
Array
(
    [1] => 11
)
Array
(
    [11] => 12
)
Array
(
    [12] => 12
)
Array
(
    [32] => 12
)
Array
(
    [11] => 12
)
Array
(
    [22] => 12
)
Array
(
    [1] => 12
)
Array
(
    [1] => 12
)
Array
(
    [11] => 1
)
Array
(
    [12] => 1
)
Array
(
    [32] => 1
)
Array
(
    [11] => 1
)
Array
(
    [22] => 1
)
Array
(
    [1] => 1
)
Array
(
    [1] => 1
)
Array
(
    [11] => 1
)
Array
(
    [12] => 1
)
Array
(
    [32] => 1
)
Array
(
    [11] => 1
)
Array
(
    [22] => 1
)
Array
(
    [1] => 1
)
Array
(
    [1] => 1
)

Fatal error: Undefined constant 'foo\foo\unknown' in %sbug42819.php on line %d