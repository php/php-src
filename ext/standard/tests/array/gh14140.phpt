--TEST--
GH-14140: Floating point bug in range operation on Apple Silicon hardware
--FILE--
<?php
/*
 * This is a problem that occurs not only in Apple silicon, but also in the Arm
 * processor environment in general, which uses clang as the compiler.
 */
print_r(range(-0.03, 0.03, 0.01));
?>
--EXPECT--
Array
(
    [0] => -0.03
    [1] => -0.02
    [2] => -0.01
    [3] => 0
    [4] => 0.01
    [5] => 0.02
    [6] => 0.03
)
