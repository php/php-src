--TEST--
Scope function returns a value
--FILE--
<?php
function test() {
    $x = 10;
    $y = 20;
    $fn = fn($a) { return $a + $x + $y; };
    var_dump($fn(5));
}
test();
?>
--EXPECT--
int(35)
