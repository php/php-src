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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    \assert(
        !
        new #[Attr(static function ($foo) {
            echo $foo;
        })]
        class {}
    );
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(!#[Attr(static function ($foo) {
    echo $foo;
})] function () {
})
assert(!new #[Attr(static function ($foo) {
    echo $foo;
})] class {
})
