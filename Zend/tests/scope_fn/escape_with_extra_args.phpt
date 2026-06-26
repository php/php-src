--TEST--
Escape detection works with extra args
--FILE--
<?php
function make($a, $b, $c) {
    return fn() { return $a + $b + $c; };
}
try {
    $fn = make(1, 2, 3);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Scope function closure must not outlive the declaring scope
