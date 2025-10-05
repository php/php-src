--TEST--
Composed callable using direct object methods.
--FILE--
<?php

$cb = new \ComposedCallable(strtolower(...));
var_dump($cb("Hello"));
$cb->prepend(fn($x) => "$x World");
var_dump($cb("Hello"));
$cb->append('strrev');
var_dump($cb("Hello"));
$cb->insert(ucfirst(...), 2);
var_dump($cb("Hello"));
--EXPECT--
string(5) "hello"
string(11) "hello world"
string(11) "dlrow olleh"
string(11) "dlrow olleH"
