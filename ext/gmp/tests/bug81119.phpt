--TEST--
GMP operators throw errors with wrong parameter names
--FILE--
<?php

function test($f) {
    try {
        $f();
        echo "No error?\n";
    } catch (TypeError|ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
test(fn($WRONG_SCOPE_1 = 0, $WRONG_SCOPE_2 = 0) => gmp_init(1) < "x");
test(fn($WRONG_SCOPE_1 = 0, $WRONG_SCOPE_2 = 0) => gmp_init(1) < []);
test(fn($WRONG_SCOPE_1 = 0, $WRONG_SCOPE_2 = 0) => gmp_init(1) + "x");
test(fn($WRONG_SCOPE_1 = 0, $WRONG_SCOPE_2 = 0) => gmp_init(1) + []);

?>
--EXPECT--
Number is not an integer string
Number must be of type GMP|string|int, array given
Number is not an integer string
Number must be of type GMP|string|int, array given
