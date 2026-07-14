--TEST--
Scope function with default parameter values
--FILE--
<?php
function test() {
    $fn = fn(int $a = 5, int $b = 10) { return $a + $b; };
    var_dump($fn());
    var_dump($fn(1));
    var_dump($fn(1, 2));
}
test();
?>
--EXPECT--
int(15)
int(11)
int(3)
