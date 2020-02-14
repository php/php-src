--TEST--
Cannot access variable scope in parameter defaults
--FILE--
<?php

namespace NS;

use Error;

function test_default($x = func_get_args()) {
    var_dump($x);
}
try {
    test_default();
} catch (Error $e) {
    printf("Caught %s on line %d\n", $e->getMessage(), $e->getLine());
}
test_default('overriding the default');
try {
    test_default();
} catch (Error $e) {
    printf("Caught %s on line %d\n", $e->getMessage(), $e->getLine());
}
?>
--EXPECTF--
Fatal error: Constant expression uses function NS\func_get_args() which is not in get_const_expr_functions() in %s on line 7
