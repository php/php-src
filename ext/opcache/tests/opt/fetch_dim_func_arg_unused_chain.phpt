--TEST--
FETCH_DIM_FUNC_ARG with UNUSED op2 followed by other FUNC_ARG fetches must not be partially converted
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function test() {
    try {
        new Node($a[]->b);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    try {
        byVal($a[][0]);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
class Node { function __construct() {} }
function byVal($x) {}
test();
?>
--EXPECT--
Cannot use [] for reading
Cannot use [] for reading
