--TEST--
013: Name conflict and functions (ns name)
--FILE--
<?php
namespace test\ns1;

function strlen($x) {
	return __FUNCTION__;
}

echo strlen("Hello"),"\n";
--EXPECT--
test\ns1\strlen
