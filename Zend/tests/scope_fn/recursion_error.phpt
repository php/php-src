--TEST--
Scope function throws on recursive call
--FILE--
<?php
function test() {
    $fn = fn() { $fn(); };
    try {
        $fn();
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}
test();
?>
--EXPECT--
Cannot recursively call scope function
