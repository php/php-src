--TEST--
empty() with multiple arguments
--FILE--
<?php

$str = 'abc';
$i = 1;

var_dump(empty($str[1], $str[3]));
var_dump(empty([1, 2, 3], 4, ''));
var_dump(empty($some_variable, false));

// short-circuiting test
empty([], ++$i);
var_dump($i);

var_dump(empty('a', true));
--EXPECT--
bool(true)
bool(true)
bool(true)
int(1)
bool(false)
