--TEST--
AST printing for FCC in attributes
--FILE--
<?php

// Do not use `false &&` to fully evaluate the function / class definition.

try {
    \assert(
        !
        #[Attr(strrev(...))]
        function () { }
    );
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    \assert(
        !
        new #[Attr(strrev(...))]
        class {}
    );
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(!#[Attr(strrev(...))] function () {
})
assert(!new #[Attr(strrev(...))] class {
})
