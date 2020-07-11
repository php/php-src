--TEST--
Test lchown() function : basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no windows support');
if (!function_exists("posix_getuid")) die("skip no posix_getuid()");
?>
--FILE--
<?php
echo "*** Testing lchown() : basic functionality ***\n";
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchown_basic.txt';
$symlink = __DIR__ . DIRECTORY_SEPARATOR . 'lchown_basic_symlink.txt';

$uid = posix_getuid();

var_dump( touch( $filename ) );
var_dump( symlink( $filename, $symlink ) );
var_dump( lchown( $filename, $uid ) );
var_dump( fileowner( $symlink ) === $uid );

?>
--CLEAN--
<?php

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchown_basic.txt';
$symlink = __DIR__ . DIRECTORY_SEPARATOR . 'lchown_basic_symlink.txt';
unlink($filename);
unlink($symlink);

?>
--EXPECT--
*** Testing lchown() : basic functionality ***
bool(true)
bool(true)
bool(true)
bool(true)
