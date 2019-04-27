--TEST--
Hint on closure with lexical vars
--FILE--
<?php
$foo = "bar";
$test = function() use($foo) : Closure {
    return function() use ($foo) {
        return $foo;
    };
};

$callable = $test();
var_dump($callable());
--EXPECT--
string(3) "bar"
