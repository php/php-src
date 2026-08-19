--TEST--
private(set) protected(set) ast printing
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

try {
    assert(function () {
        class Foo {
            public private(set) string $bar;
            public protected(set) string $baz;
        }
    } && false);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(function () {
    class Foo {
        public private(set) string $bar;
        public protected(set) string $baz;
    }

} && false)
