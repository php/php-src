--TEST--
OSS-Fuzz #447521098: Fatal error during sccp shift eval
--FILE--
<?php
function test() {
    $x = 0;
    $y = -1;
    $x >> $y;
}
?>
===DONE===
--EXPECT--
===DONE===
