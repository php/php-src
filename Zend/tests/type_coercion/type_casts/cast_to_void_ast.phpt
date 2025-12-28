--TEST--
(void) is included in AST printing
--FILE--
<?php

try {
    assert(false && function () {
        (void) somefunc();
    });
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && function () {
    (void)somefunc();
})
