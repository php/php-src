--TEST--
036: Name ambiguity in compile-time constant reference (ns name)
--SKIPIF--
<?php if (!extension_loaded("spl")) die("skip SPL is no available"); ?>
--FILE--
<?php
namespace A;
use A as B;
class ArrayObject {
	const STD_PROP_LIST = 2;
}
function f1($x = ArrayObject::STD_PROP_LIST) {
	var_dump($x);
}
function f2($x = \ArrayObject::STD_PROP_LIST) {
	var_dump($x);
}
function f3($x = \A\ArrayObject::STD_PROP_LIST) {
	var_dump($x);
}
function f4($x = B\ArrayObject::STD_PROP_LIST) {
	var_dump($x);
}
var_dump(ArrayObject::STD_PROP_LIST);
var_dump(\ArrayObject::STD_PROP_LIST);
var_dump(B\ArrayObject::STD_PROP_LIST);
var_dump(\A\ArrayObject::STD_PROP_LIST);
f1();
f2();
f3();
f4();
?>
--EXPECT--
int(2)
int(1)
int(2)
int(2)
int(2)
int(1)
int(2)
int(2)
