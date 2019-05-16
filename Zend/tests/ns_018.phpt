--TEST--
018: __NAMESPACE__ constant and runtime names (ns name)
--FILE--
<?php
namespace test;

function foo() {
	return __FUNCTION__;
}

$x = __NAMESPACE__ . "\\foo";
echo $x(),"\n";
--EXPECT--
test\foo
