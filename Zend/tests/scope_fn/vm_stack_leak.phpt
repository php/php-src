--TEST--
Repeated scope function calls don't leak VM stack frames
--FILE--
<?php
function test() {
    $sum = 0;
    $fn = fn($v) { $sum += $v; };
    for ($i = 0; $i < 10000; $i++) { $fn($i); }
    var_dump($sum);
}
test();
?>
--EXPECT--
int(49995000)
