--TEST--
chmod() with various paths
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

define("PERMISSIONS_MASK", 0777);

$script_directory = dirname(__FILE__);
chdir($script_directory);
$test_dirname = basename(__FILE__, ".php") . "testdir";
mkdir($test_dirname);

$filepath = __FILE__ . ".tmp";
$filename = basename($filepath);
$fd = fopen($filepath, "w+");
fclose($fd);

echo "chmod() on a path containing .. and .\n";
var_dump(chmod("./$test_dirname/../$filename", 0777));
var_dump(chmod("./$test_dirname/../$filename", 0755));
clearstatcache();
printf("%o\n", fileperms($filepath) & PERMISSIONS_MASK);

echo "\nchmod() on a path containing .. with invalid directories\n";
var_dump(chmod($filepath, 0777));
var_dump(chmod("./$test_dirname/bad_dir/../../$filename", 0755));
clearstatcache();
printf("%o\n", fileperms($filepath) & PERMISSIONS_MASK);
 
echo "\nchmod() on a linked file\n";
$linkname = "somelink";
var_dump(symlink($filepath, $linkname));
var_dump(chmod($filepath, 0777));
var_dump(chmod($linkname, 0755));
clearstatcache();
printf("%o\n", fileperms($filepath) & PERMISSIONS_MASK);
var_dump(unlink($linkname));

echo "\nchmod() on a relative path from a different working directory\n";
chdir($test_dirname);
var_dump(chmod("../$filename", 0777));
var_dump(chmod("../$filename", 0755));
clearstatcache();
printf("%o\n", fileperms($filepath) & PERMISSIONS_MASK);
chdir($script_directory);

echo "\nchmod() on a directory with a trailing /\n";
var_dump(chmod($test_dirname, 0777));
var_dump(chmod("$test_dirname/", 0775));
clearstatcache();
printf("%o\n", fileperms($filepath) & PERMISSIONS_MASK);

chdir($script_directory);
rmdir($test_dirname);
unlink($filepath);

?>
--EXPECTF--
chmod() on a path containing .. and .
bool(true)
bool(true)
755

chmod() on a path containing .. with invalid directories
bool(true)

Warning: chmod(): No such file or directory in %s on line %d
bool(false)
777

chmod() on a linked file
bool(true)
bool(true)
bool(true)
755
bool(true)

chmod() on a relative path from a different working directory
bool(true)
bool(true)
755

chmod() on a directory with a trailing /
bool(true)
bool(true)
755
