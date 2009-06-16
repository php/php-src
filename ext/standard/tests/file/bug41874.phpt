--TEST--
bug #42143 (The constant NAN is reported as 0 on Windows build)
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
		die('skip windows only test');
?>
--FILE--
<?php
$result = exec('cd 1:\non_existant; dir nonexistant');
echo "$result";
system('cd 1:\non_existant; dir nonexistant');
?>
--EXPECT--
The system cannot find the drive specified.
The system cannot find the drive specified.