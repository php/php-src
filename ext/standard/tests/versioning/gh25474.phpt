--TEST--
GH-25474 (version_compare() treats numeric parts > 2147483647 as equal on Windows)
--CREDITS--
HFigarella
--FILE--
<?php
var_dump(version_compare('2147483648', '2147483647'));
var_dump(version_compare('5000000000', '6000000000'));
var_dump(version_compare('16.5-a.2-43779010004', '16.5-a.2-43192679404'));
?>
--EXPECT--
int(1)
int(-1)
int(1)
