--TEST--
Scope function reads parent variable
--FILE--
<?php
function test() {
    $x = 42;
    $fn = fn($a) { return $a + $x; };
    var_dump($fn(10));
}
test();
?>
--EXPECT--
int(52)
