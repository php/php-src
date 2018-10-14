--TEST--
Test filetype() function: Check character type
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no /dev on Windows');
}
if (!file_exists("/dev/null")) {
    die('skip /dev/null not available');
}
?>
--FILE--
<?php
/*
Prototype: string filetype ( string $filename );
Description: Returns the type of the file. Possible values are fifo, char,
             dir, block, link, file, and unknown.
*/

echo "-- Checking for char --\n";
print( filetype("/dev/null") )."\n";
?>
===DONE===
--EXPECTF--
-- Checking for char --
char
===DONE===
