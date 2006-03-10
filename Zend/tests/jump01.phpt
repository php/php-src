--TEST--
jump 01: jump backward
--FILE--
<?php
$n = 1;
L1:
echo "$n: ok\n";
$n++;
if ($n <= 3) jump L1;
?>
--EXPECT--
1: ok
2: ok
3: ok
