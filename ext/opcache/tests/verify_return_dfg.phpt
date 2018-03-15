--TEST--
Incorrect liveness computation for verify-return
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
