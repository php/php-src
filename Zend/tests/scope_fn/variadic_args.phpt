--TEST--
Scope function with variadic parameters
--FILE--
<?php
function test() {
    $fn = fn(int ...$args): int { return array_sum($args); };
    var_dump($fn(1, 2, 3));
}
test();
?>
--EXPECT--
int(6)
