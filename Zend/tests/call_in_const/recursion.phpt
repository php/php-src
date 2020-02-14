--TEST--
Recursion in calls in class constants causes an error
--FILE--
<?php
function x_plus_1() {
    echo "Computing X + 1\n";
    return Recursion::X + 1;
}
class Recursion {
    const X = x_plus_1();
    const MISSING = MISSING_GLOBAL + 1;
}
try {
    echo "Recursion::X=" . Recursion::X . "\n";
} catch (Error $e) {
    printf("Caught %s: %s\n", get_class($e), $e->getMessage());
}
try {
    echo "Recursion::X=" . Recursion::X . "\n";
} catch (Error $e) {
    printf("Second call caught %s: %s\n", get_class($e), $e->getMessage());
}
try {
    echo "Recursion::MISSING=" . Recursion::MISSING;
} catch (Error $e) {
    printf("Caught %s: %s\n", get_class($e), $e->getMessage());
}
try {
    echo "Recursion::MISSING=" . Recursion::MISSING;
} catch (Error $e) {
    printf("Second call caught %s: %s\n", get_class($e), $e->getMessage());
}
?>
--EXPECT--
Fatal error: Constant expression uses function x_plus_1() which is not in get_const_expr_functions() in /home/tyson/programming/php-src/Zend/tests/call_in_const/recursion.php on line 7