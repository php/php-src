--TEST--
Yield from non-iterable
--FILE--
<?php

function gen() {
    yield from new stdClass;
}

try {
    gen()->current();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Can use "yield from" only with arrays and Traversables
