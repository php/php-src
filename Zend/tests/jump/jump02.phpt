--TEST--
jump 02: goto forward
--FILE--
<?php
$n = 1;
L1:
if ($n > 3) goto L2;
echo "$n: ok\n";
$n++;
goto L1;
L2:
?>
--EXPECT--
1: ok
2: ok
3: ok
