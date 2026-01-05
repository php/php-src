--TEST--
Pinning pattern
--FILE--
<?php

$a = 1;
$b = '1';
$c = 'foo';

var_dump(1 is ^$a);
var_dump(2 is ^$a);
var_dump(1 is ^$b);
var_dump('foo' is ^$c);
var_dump('bar' is ^$c);
var_dump('1' is ^$a);

const A = 1;
const B = '1';
const C = 'foo';

var_dump(1 is ^A);
var_dump(2 is ^A);
var_dump(1 is ^B);
var_dump('foo' is ^C);
var_dump('bar' is ^C);
var_dump('1' is ^A);

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
