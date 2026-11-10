--TEST--
AST printing of interface extends list
--INI--
zend.assertions=1
--FILE--
<?php

try {
    assert(false && function () {
        interface I extends A, B {}
    });
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && function () {
    interface I extends A, B {
    }

})
