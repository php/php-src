--TEST--
Create a class instance with a string
--FILE--
<?php

class Foo {
}

$foo = new Foo;

var_dump(is_object($foo));
echo get_class($foo) . PHP_EOL;

echo 'Done' . PHP_EOL;
?>
--EXPECTF--	
bool(true)
Foo
Done
