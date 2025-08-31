--TEST--
Printing AST of exit "constant" via assert
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(0 && exit);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
AssertionError: assert(0 && \exit())
