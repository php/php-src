--TEST--
Too few arguments to scope function
--FILE--
<?php
function test() {
    $fn = fn(int $a, int $b) { return $a + $b; };
    try {
        $fn(1);
    } catch (Error $e) {
        echo "Caught: " . $e->getMessage() . "\n";
    }
    echo "ok\n";
}
test();
?>
--EXPECTF--
Caught: Too few arguments to function %s, 1 passed %s and exactly 2 expected
ok
