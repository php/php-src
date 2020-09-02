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
echo "-- Checking for char --\n";
print( filetype("/dev/null") )."\n";
?>
--EXPECT--
-- Checking for char --
char
