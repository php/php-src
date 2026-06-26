--TEST--
Scope function in conditional branch that is not taken does not crash
--FILE--
<?php
function test() {
    if (false) {
        $fn = fn() { return 1; };
    }
    return "ok";
}
var_dump(test());
?>
--EXPECT--
string(2) "ok"
