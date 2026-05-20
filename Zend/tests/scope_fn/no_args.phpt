--TEST--
Scope function with no arguments
--FILE--
<?php
function test() {
    $x = 42;
    $fn = fn() { return $x; };
    var_dump($fn());
}
test();
?>
--EXPECT--
int(42)
