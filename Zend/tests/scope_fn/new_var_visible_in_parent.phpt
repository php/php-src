--TEST--
New variable created in scope function is visible in parent
--FILE--
<?php
function test() {
    $fn = fn() { $newvar = "hello"; };
    $fn();
    var_dump($newvar);
}
test();
?>
--EXPECT--
string(5) "hello"
