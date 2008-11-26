--TEST--
Test filetype() function: Variations
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no /dev on Windows');
}
?>
--FILE--
<?php
/*
Prototype: string filetype ( string $filename );
Description: Returns the type of the file. Possible values are fifo, char,
             dir, block, link, file, and unknown. 
*/

echo "*** Testing filetype() with various types ***\n";
echo "-- Checking for char --\n";
print( filetype("/dev/console") )."\n";

echo "-- Checking for block --\n";
//we have cheated in our mac build by creating a ram0 block device.
print( filetype("/dev/ram0") )."\n";

echo "\n*** Done ***\n";
?>
--EXPECTF--
*** Testing filetype() with various types ***
-- Checking for char --
char
-- Checking for block --
block

*** Done ***
