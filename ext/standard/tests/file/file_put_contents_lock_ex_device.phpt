--TEST--
file_put_contents() LOCK_EX permits writes to a device that cannot be truncated
--FILE--
<?php
$device = PHP_OS_FAMILY === 'Windows' ? 'NUL' : '/dev/null';
var_dump(file_put_contents($device, 'data', LOCK_EX));
var_dump(file_put_contents($device, '', LOCK_EX));
?>
--EXPECT--
int(4)
int(0)
