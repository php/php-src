--TEST--
Printing AST of die function via assert
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(0 && die());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
AssertionError: assert(0 && \exit())
