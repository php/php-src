--TEST--
Test lchgrp() function : basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no windows support');
if (!function_exists("posix_getgid")) die("skip no posix_getgid()");
?>
--FILE--
<?php
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchgrp.txt';
$symlink = __DIR__ . DIRECTORY_SEPARATOR . 'symlink.txt';

$gid = posix_getgid();

var_dump( touch( $filename ) );
var_dump( symlink( $filename, $symlink ) );
var_dump( lchgrp( $filename, $gid ) );
var_dump( filegroup( $symlink ) === $gid );

?>
--CLEAN--
<?php

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchgrp.txt';
$symlink = __DIR__ . DIRECTORY_SEPARATOR . 'symlink.txt';
unlink($filename);
unlink($symlink);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
