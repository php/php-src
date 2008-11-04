--TEST--
016: Run-time name conflict and functions (ns name)
--FILE--
<?php
namespace test\ns1;

function strlen($x) {
	return __FUNCTION__;
}

$x = "test\\ns1\\strlen";
echo $x("Hello"),"\n";
--EXPECT--
test\ns1\strlen
