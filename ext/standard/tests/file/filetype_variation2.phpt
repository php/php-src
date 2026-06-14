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
$devnull = "/dev/null";
// On Solaris, /dev/null is symlink
if (is_link($devnull)) {
    $devnull = readlink($devnull);
    if ($devnull[0] != '/') $devnull = dirname("/dev/null") . '/' . $devnull;
}
echo "-- Checking for char --\n";
print(filetype($devnull)) . "\n";
?>
--EXPECT--
-- Checking for char --
char
