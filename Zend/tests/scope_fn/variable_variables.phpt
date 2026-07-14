--TEST--
Variable variables inside scope function access parent scope
--FILE--
<?php
function test() {
    $x = 42;
    $fn = fn() { $name = "x"; return $$name; };
    var_dump($fn());
}
test();
?>
--EXPECT--
int(42)
