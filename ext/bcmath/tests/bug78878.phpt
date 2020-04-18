--TEST--
Bug #78878 (Buffer underflow in bc_shift_addsub)
--SKIPIF--
<?php
if (!extension_loaded('bcmath')) die('skip bcmath extension not available');
?>
--FILE--
<?php
print @bcmul("\xB26483605105519922841849335928742092", bcpowmod(2, 65535, -4e-4));
?>
--EXPECT--
0
