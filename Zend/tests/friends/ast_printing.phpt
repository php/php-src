--TEST--
Friends: AST printing
--FILE--
<?php

try {
    assert(function () {
        class Foo {
			friend Bar;
        }
    } && false);
} catch (Error $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
assert(function () {
    class Foo {
        friend Bar;
    }

} && false)
