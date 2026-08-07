--TEST--
Generators can throw exceptions
--FILE--
<?php

function gen() {
    yield 'foo';
    throw new Exception('test');
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
