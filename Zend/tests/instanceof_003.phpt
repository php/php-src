--TEST--
Testing instanceof operator with expression
--FILE--
<?php

class Foo {
}

$foo = new Foo;

$oo = 'oo';
$ar = 'ar';

var_dump($foo instanceof {'f' . $oo});
var_dump($foo instanceof {'b' . $ar});

echo 'Done' . PHP_EOL;
?>
--EXPECTF--	
bool(true)
bool(false)
Done
