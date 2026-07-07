--TEST--
Pipe assign operator basic usage with first-class callable
--FILE--
<?php

$x = "hello";
$x |>= strtoupper(...);
var_dump($x);

?>
--EXPECT--
string(5) "HELLO"
