--TEST--
052: Name conflict and compile-time constants (php name in case if ns name exists)
--FILE--
<?php
namespace test\ns1;

const INI_ALL = 0;

function foo($x = \INI_ALL) {
	var_dump($x);
}
foo();
--EXPECT--
int(7)
