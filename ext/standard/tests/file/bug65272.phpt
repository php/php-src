--TEST--
Bug #65272: flock() correctly sets wouldblock out param in windows
--SKIPIF--
<?php
if (stripos(PHP_OS, 'win') !== 0) die("skip windows required");
?>
--FILE--
<?php

$file = dirname(__FILE__)."/flock_bug65272.dat";

$fp1 = fopen($file, "w");
var_dump(flock($fp1, LOCK_SH));

$fp2 = fopen($file, "r");
var_dump(flock($fp2, LOCK_EX|LOCK_NB, $wouldblock));
var_dump($wouldblock);

echo "Done\n";
?>
--CLEAN--
<?php
$file = dirname(__FILE__)."/flock_bug65272.dat";
unlink($file);
?>
--EXPECTF--
bool(true)
bool(false)
int(1)
Done
