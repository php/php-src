--TEST--
exec, system, passthru  — Basic command execution functions
--SKIPIF--
<?php
// If this does not work for Windows, please uncomment or fix test
// if(substr(PHP_OS, 0, 3) == "WIN") die("skip not for Windows");
?>
--FILE--
<?php
$cmd = "echo abc\n\0command";
var_dump(exec($cmd, $output));
var_dump($output);
var_dump(system($cmd));
var_dump(passthru($cmd));
?>
--EXPECTF--
Warning: exec(): NULL byte detected. Possible attack in %s on line %d
bool(false)
NULL

Warning: system(): NULL byte detected. Possible attack in %s on line %d
bool(false)

Warning: passthru(): NULL byte detected. Possible attack in %s on line %d
bool(false)
