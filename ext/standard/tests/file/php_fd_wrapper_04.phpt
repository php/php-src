--TEST--
php://fd wrapper: invalid file descriptor
--SKIPIF--
<?php
// At least on Mac 10.14 Mojave; not sure about older versions
if (strtolower(PHP_OS) == 'darwin') die('skip: the warning output is different on MacOS');
?>
--FILE--
<?php
fopen("php://fd/1023", "w");

echo "\nDone.\n";
?>
--EXPECTF--
Warning: fopen(php://fd/1023): Failed to open stream: Error duping file descriptor 1023; possibly it doesn't exist: [9]: %s in %s on line %d

Done.
