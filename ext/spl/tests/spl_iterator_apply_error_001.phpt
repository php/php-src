--TEST--
SPL: Error: iterator_apply when the callback throws an exception
--FILE--
<?php

function test() {
    throw new Exception('Broken callback');
}

$it = new RecursiveArrayIterator(array(1, 21, 22));

try {
    iterator_apply($it, 'test');
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Exception: Broken callback
