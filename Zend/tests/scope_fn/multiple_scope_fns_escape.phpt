--TEST--
Multiple scope functions - one escapes
--FILE--
<?php
function test() {
    $add = fn($a) { return $a + 1; };
    $mul = fn($a) { return $a * 2; };
    var_dump($add(5));
    var_dump($mul(5));
    $GLOBALS["escaped"] = $mul;
}
try {
    test();
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
int(6)
int(10)
Scope function closure must not outlive the declaring scope
