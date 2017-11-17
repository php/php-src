--TEST--
call_user_func() should error on reference arguments
--FILE--
<?php

namespace Foo;

function bar(&$ref) {
    $ref = 24;
}

$x = 42;
$ref =& $x;
\call_user_func('Foo\bar', $x);
var_dump($x);

$y = 42;
$ref =& $y;
call_user_func('Foo\bar', $y);
var_dump($y);

?>
--EXPECTF--
Warning: Parameter 1 to Foo\bar() expected to be a reference, value given in %s on line %d
int(42)

Warning: Parameter 1 to Foo\bar() expected to be a reference, value given in %s on line %d
int(42)
