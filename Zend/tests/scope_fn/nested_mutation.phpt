--TEST--
Nested scope function mutates grandparent variable
--FILE--
<?php
function outer() {
    $x = 1;
    $inner = fn() {
        $nested = fn() { $x = 42; };
        $nested();
    };
    $inner();
    var_dump($x); // 42
}
outer();
?>
--EXPECT--
int(42)
