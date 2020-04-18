--TEST--
bug #41874 (Separate STDOUT and STDERR in exec functions)
--CREDITS--
Venkat Raman Don
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) != 'WIN' ) {
		die('skip windows only test');
}
?>
--FILE--
<?php
$result = exec('cd 1:\\non_existent; dir nonexistent');
echo "$result";
?>
--EXPECT--
The system cannot find the drive specified.
