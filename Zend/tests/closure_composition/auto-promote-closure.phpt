--TEST--
Closures get auto promoted to a Composed Closure.
--FILE--
<?php

$cb = strtolower(...) + str_rot13(...) + (fn($x) => "<$x>");
var_dump($cb("Hello World"));
--EXPECT--
string(13) "<uryyb jbeyq>"
