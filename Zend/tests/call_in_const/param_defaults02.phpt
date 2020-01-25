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
--EXPECT--
Caught Cannot call func_get_args() dynamically on line 7
string(22) "overriding the default"
Caught Cannot call func_get_args() dynamically on line 7
