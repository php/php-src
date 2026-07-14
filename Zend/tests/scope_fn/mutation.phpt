--TEST--
Scope function modifies parent variable
--FILE--
<?php
function test() {
    $x = 1;
    $fn = fn() { $x = 99; };
    $fn();
    var_dump($x);
}
test();
?>
--EXPECT--
int(99)
