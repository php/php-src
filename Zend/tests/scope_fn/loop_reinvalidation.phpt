--TEST--
Re-evaluating scope function in loop invalidates previous instance
--FILE--
<?php
function test() {
    $closures = [];
    for ($i = 0; $i < 3; $i++) {
        $fn = fn() { return $i; };
        $closures[] = $fn;
    }
    // Latest closure works
    var_dump($closures[2]());
    // Earlier closures were invalidated by re-evaluation
    try {
        $closures[0]();
    } catch (Error $e) {
        echo $e->getMessage() . "\n";
    }
}
test();
?>
--EXPECT--
int(3)
Cannot call scope function: defining scope has exited
