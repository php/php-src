--TEST--
GH-11320: list() expressions can contain magic constants
--FILE--
<?php
[list(__FILE__ => $foo) = [__FILE__ => 'foo']];
var_dump($foo);
[[__FILE__ => $foo] = [__FILE__ => 'foo']];
var_dump($foo);
?>
--EXPECT--
string(3) "foo"
string(3) "foo"
