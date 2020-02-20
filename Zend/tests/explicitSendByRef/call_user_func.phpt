--TEST--
call_user_func() with explicit pass by ref
--FILE--
<?php

// Avoid VM builtin.
namespace Foo;

function inc(&$i) { $i++; }

$i = 0;
call_user_func('Foo\inc', $i);
var_dump($i);

$i = 0;
call_user_func('Foo\inc', &$i);
var_dump($i);

$i = 0;
\call_user_func('Foo\inc', $i);
var_dump($i);

$i = 0;
\call_user_func('Foo\inc', &$i);
var_dump($i);

?>
--EXPECTF--
Warning: Foo\inc() expects argument #1 ($i) to be passed by reference, value given in %s on line %d
int(0)
int(1)

Warning: Foo\inc() expects argument #1 ($i) to be passed by reference, value given in %s on line %d
int(0)
int(1)
