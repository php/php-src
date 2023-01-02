--TEST--
Unreachable phi cycle
--FILE--
<?php
// The inner loop is dead, but SCCP reachability analysis doesn't realize this,
// as this is determined based on type information.
function test() {
    for (; $i--;) {
        for(; x;);
    }
}
test();
?>
--EXPECTF--
Warning: Undefined variable $i (this will become an error in PHP 9.0) in %s on line %d
