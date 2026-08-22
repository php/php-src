--TEST--
AST printing for closures in attributes
--FILE--
<?php

// Do not use `false &&` to fully evaluate the function / class definition.

try {
    \assert(
        !
        #[Attr(static function ($foo) {
            echo $foo;
        })]
        function () { }
    );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    \assert(
        !
        new #[Attr(static function ($foo) {
            echo $foo;
        })]
        class {}
    );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(!#[Attr(static function ($foo) {
    echo $foo;
})] function () {
})
AssertionError: assert(!new #[Attr(static function ($foo) {
    echo $foo;
})] class {
})
