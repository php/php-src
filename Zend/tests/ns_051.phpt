--TEST--
051: Name conflict and compile-time constants (php name)
--FILE--
<?php
namespace test\ns1;

function foo($x = INI_ALL) {
	var_dump($x);
}
foo();
--EXPECT--
int(7)
