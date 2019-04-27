--TEST--
009: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php
class foo {
}

$x = __NAMESPACE__ . "\\foo";
echo get_class(new $x),"\n";
--EXPECT--
foo
