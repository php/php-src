--TEST--
Capture properties with different values in multiple instances of the same class
--FILE--
<?php

function make_anon_class(int $foo) {
    return new class use ($foo as int) {};
}

$one = make_anon_class(1);
$two = make_anon_class(2);

var_dump($one->foo);
var_dump($two->foo);
var_dump($one instanceof $two);

?>
--EXPECT--
int(1)
int(2)
bool(true)
