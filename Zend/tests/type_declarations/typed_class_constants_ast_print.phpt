--TEST--
AST printing support for typed constants
--FILE--
<?php

try {
    assert(false && new class {
        public const int X = 1;
    });
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && new class {
    public const int X = 1;
})
