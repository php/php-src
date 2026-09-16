--TEST--
AST printing support for typed constants
--FILE--
<?php

try {
    assert(false && new class {
        public const int X = 1;
    });
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(false && new class {
    public const int X = 1;
})
