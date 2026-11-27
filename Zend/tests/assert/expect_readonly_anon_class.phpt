--TEST--
AST export of readonly anonymous class
--INI--
zend.assertions=1
--FILE--
<?php
try {
    assert(false && new readonly class {});
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    assert(false && new #[A] readonly class(1) {});
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
AssertionError: assert(false && new readonly class {
})
AssertionError: assert(false && new #[A] readonly class(1) {
})
