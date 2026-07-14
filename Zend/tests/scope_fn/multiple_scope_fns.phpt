--TEST--
Multiple scope functions in same parent
--FILE--
<?php
function test() {
    $x = 10;
    $add = fn($a) { return $x + $a; };
    $mul = fn($a) { return $x * $a; };
    var_dump($add(5));
    var_dump($mul(5));
}
test();
?>
--EXPECT--
int(15)
int(50)
