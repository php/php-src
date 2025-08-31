--TEST--
Test lchown() function : error functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die('skip no windows support');
if (!function_exists("posix_getuid")) die("skip no posix_getuid()");
require __DIR__ . '/../skipif_root.inc';
?>
--FILE--
<?php
echo "*** Testing lchown() : error functionality ***\n";

// Set up
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchown.txt';
touch( $filename );
$uid = posix_getuid();

// Non-existent filename
var_dump( lchown( 'foobar_lchown.txt', $uid ) );

// Bad user
var_dump( lchown( $filename, -5 ) );

?>
--CLEAN--
<?php

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchown.txt';
unlink($filename);

?>
--EXPECTF--
*** Testing lchown() : error functionality ***

Warning: lchown(): No such file or directory in %s on line %d
bool(false)

Warning: lchown(): %r(Operation not permitted|Invalid argument)%r in %s on line %d
bool(false)
