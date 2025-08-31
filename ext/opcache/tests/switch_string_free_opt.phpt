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
--EXPECT--
===DONE===
