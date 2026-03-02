--TEST--
Handling of pi nodes when replacing a predecessor
--FILE--
<?php
function test(bool $a, bool $b) {
    $byte = '';
    if ($a && $byte > 0 && $b) {}
    unknown($byte);
}
function test2() {
    foreach (0 as $v) {
        $v ??= 0;
    }
}
?>
===DONE===
--EXPECT--
===DONE===
