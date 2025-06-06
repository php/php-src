--TEST--
Composed callable using composition and auto-promotion
--FILE--
<?php

$cb = strtolower(...) + str_rot13(...) + (fn($x) => "<$x>");
var_dump($cb("Hello World"));
--EXPECT--
string(13) "<uryyb jbeyq>"
