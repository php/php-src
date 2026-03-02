--TEST--
Bug #71201 round() segfault on 64-bit builds
--FILE--
<?php
echo round(1.0, -2147483648), "\n";
?>
--EXPECT--
0
