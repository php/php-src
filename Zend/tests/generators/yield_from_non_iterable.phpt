--TEST--
Yield from non-iterable
--FILE--
<?php

function gen() {
    yield from new stdClass;
}

try {
    gen()->current();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Can use "yield from" only with arrays and Traversables
