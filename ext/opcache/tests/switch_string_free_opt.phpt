--TEST--
A SWITCH_STRING operand FREE may be optimized away
--FILE--
<?php
function test($a) {
    switch (!$a) {
        case '':
            r>l;
        default:
    }
}
?>
===DONE===
--EXPECTF--
Warning: Non-empty case falls through to the next case, terminate the case with "fallthrough;" if this is intentional in %s on line %d
===DONE===
