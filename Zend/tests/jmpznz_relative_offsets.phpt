--TEST--
Check for correct treatment of relative JMPZNZ offsets when copying opline
--FILE--
<?php
function test($c) {
    L1:
    if ($c) {
        goto L1;
        goto L1;
    }
}
test(false);
?>
===DONE===
--EXPECT--
===DONE===
