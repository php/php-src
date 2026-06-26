--TEST--
Exception thrown inside scope function propagates to caller
--FILE--
<?php
function test() {
    $x = 42;
    $fn = fn() { throw new RuntimeException("boom"); };
    try {
        $fn();
    } catch (RuntimeException $e) {
        echo "Caught: " . $e->getMessage() . "\n";
    }
    var_dump($x);
}
test();
?>
--EXPECT--
Caught: boom
int(42)
