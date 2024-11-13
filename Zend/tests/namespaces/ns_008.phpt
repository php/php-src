--TEST--
008: __NAMESPACE__ constant and runtime names (ns name)
--FILE--
<?php
namespace test;

class foo {
}

$x = __NAMESPACE__ . "\\foo";
echo get_class(new $x),"\n";
?>
--EXPECT--
test\foo
