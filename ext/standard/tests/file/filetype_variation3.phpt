--TEST--
Test filetype() function: Check block device
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no /dev on Windows');
}
if (!file_exists("/dev/ram0")) {
    die('skip /dev/ram0 not available');
}
?>
--FILE--
<?php
/*
Prototype: string filetype ( string $filename );
Description: Returns the type of the file. Possible values are fifo, char,
             dir, block, link, file, and unknown. 
*/

echo "-- Checking for block --\n";
print( filetype("/dev/ram0") )."\n";
?>
===DONE===
--EXPECTF--
-- Checking for block --
block
===DONE===
