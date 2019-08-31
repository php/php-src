--TEST--
015: Name conflict and functions (php name in case if ns name exists)
--FILE--
<?php
namespace test\ns1;

function strlen($x) {
	return __FUNCTION__;
}

echo \strlen("Hello"),"\n";
--EXPECT--
5
