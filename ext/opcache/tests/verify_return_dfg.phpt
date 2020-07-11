--TEST--
Incorrect liveness computation for verify-return
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function test($foo): string
{
    switch ($foo) {
        default:  $bar = 'x'; break;
        case 'z': $bar = 'y'; break;
    }
    return (string)$bar;
}
?>
===DONE===
--EXPECT--
===DONE===
