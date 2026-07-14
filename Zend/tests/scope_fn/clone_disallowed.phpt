--TEST--
Scope function closure cannot be cloned
--FILE--
<?php
function test() {
    $fn = fn() { return 1; };
    try {
        $fn2 = clone $fn;
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}
test();
?>
--EXPECT--
Cannot clone a scope function closure
