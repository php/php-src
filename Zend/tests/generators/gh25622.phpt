--TEST--
GH-25622 (Use-after-free in zend_generator_dtor_storage() when a delegate's finally block resumes the delegating generator)
--CREDITS--
rafaelscheel
--FILE--
<?php

function inner() {
    try {
        yield 1;
    } finally {
        try {
            $GLOBALS['gen']->throw(new Exception('from finally'));
        } catch (Exception $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
}

function outer() {
    yield from inner();
}

$gen = outer();
$ref = $gen;
var_dump($gen->current());

?>
--EXPECT--
int(1)
Exception: from finally
