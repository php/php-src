--TEST--
Composed callable using composition.
--FILE--
<?php

$cb1 = new \ComposedCallable([strtolower(...)]);
$cb2 = $cb1 + strrev(...);
var_dump($cb1("Hello World"));
var_dump($cb2("Hello World"));
--EXPECT--
string(11) "hello world"
string(11) "dlrow olleh"
