--TEST--
Don't optimize send opcodes that differ in by-ref behavior
--FILE--
<?php

call_user_func('ref', function_exists('strlen'));
ref(function_exists('strlen'));

function ref(&$x) {
    var_dump($x);
}

?>
--EXPECTF--
Warning: ref(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
bool(true)

Notice: Only variables should be passed by reference in %s on line %d
bool(true)
