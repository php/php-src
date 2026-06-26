--TEST--
fn() use() {} is disallowed for scope functions (parse error)
--FILE--
<?php
function test() {
    $x = 1;
    $fn = fn() use ($x) { return $x; };
}
?>
--EXPECTF--
Parse error: syntax error, unexpected token "use", expecting %s in %s on line %d
