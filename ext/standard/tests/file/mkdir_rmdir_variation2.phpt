--TEST--
Test mkdir() and rmdir() functions: usage variations - misc.
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only on LINUX');
}
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php

$context = stream_context_create();

$file_path = __DIR__;

echo "\n*** Testing mkdir() and rmdir() by giving stream context as fourth argument ***\n";
var_dump( mkdir("$file_path/mkdir_variation2/test/", 0777, true, $context) );
var_dump( rmdir("$file_path/mkdir_variation2/test/", $context) );

echo "\n*** Testing rmdir() on a non-empty directory ***\n";
var_dump( mkdir("$file_path/mkdir_variation2/test/", 0777, true) );
var_dump( rmdir("$file_path/mkdir_variation2/") );

echo "\n*** Testing mkdir() and rmdir() for binary safe functionality ***\n";
try {
    var_dump( mkdir("$file_path/temp".chr(0)."/") );
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( rmdir("$file_path/temp".chr(0)."/") );
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\n*** Testing mkdir() with miscellaneous input ***\n";
/* changing mode of mkdir to prevent creating sub-directory under it */
var_dump( chmod("$file_path/mkdir_variation2/", 0000) );
/* creating sub-directory test1 under mkdir, expected: false */
var_dump( mkdir("$file_path/mkdir_variation2/test1", 0777, true) );
var_dump( chmod("$file_path/mkdir_variation2/", 0777) );  // chmod to enable removing test1 directory

echo "Done\n";
?>
--CLEAN--
<?php
rmdir(__DIR__."/mkdir_variation2/test/");
rmdir(__DIR__."/mkdir_variation2/");
?>
--EXPECTF--
*** Testing mkdir() and rmdir() by giving stream context as fourth argument ***
bool(true)
bool(true)

*** Testing rmdir() on a non-empty directory ***
bool(true)

Warning: rmdir(%s/mkdir_variation2/): %s on line %d
bool(false)

*** Testing mkdir() and rmdir() for binary safe functionality ***
mkdir(): Argument #1 ($directory) must not contain any null bytes
rmdir(): Argument #1 ($directory) must not contain any null bytes

*** Testing mkdir() with miscellaneous input ***
bool(true)

Warning: mkdir(): Permission denied in %s on line %d
bool(false)
bool(true)
Done
