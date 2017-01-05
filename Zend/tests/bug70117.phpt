--TEST--
Bug #70117 (Unexpected return type error)
--FILE--
<?php

function &foo() :string {
	$a = array(1);
	$b = &$a[0];
	return $b;
}

function &foo1() :string {
	$a = array("ref");
	return $a[0];
}

var_dump(foo());
var_dump(foo1());
--EXPECT--
string(1) "1"
string(3) "ref"
