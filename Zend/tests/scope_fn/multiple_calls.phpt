--TEST--
Scope function can be called multiple times
--FILE--
<?php
function test() {
    $counter = 0;
    $fn = fn() { $counter++; };
    $fn();
    $fn();
    $fn();
    var_dump($counter);
}
test();
?>
--EXPECT--
int(3)
