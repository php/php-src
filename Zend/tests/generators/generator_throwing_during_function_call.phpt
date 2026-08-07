--TEST--
Stack is cleaned up properly when an exception is thrown during a function call
--FILE--
<?php

function throwException() {
    throw new Exception('test');
}

function gen() {
    yield 'foo';
    strlen("foo", "bar", throwException());
    yield 'bar';
}

$gen = gen();

var_dump($gen->current());

try {
    $gen->next();
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($gen->current());

?>
--EXPECT--
string(3) "foo"
Exception: test
NULL
