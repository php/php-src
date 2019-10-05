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
/* Prototype  : bool lchown (string filename, mixed user)
 * Description: Change file owner of a symlink
 * Source code: ext/standard/filestat.c
 * Alias to functions:
 */

echo "*** Testing lchown() : error functionality ***\n";

// Set up
$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchown.txt';
touch( $filename );
$uid = posix_getuid();


// Less than expected arguments
var_dump( lchown( $filename ) );

// More than expected arguments
var_dump( lchown( $filename, $uid, 'foobar' ) );

// Non-existent filename
var_dump( lchown( 'foobar_lchown.txt', $uid ) );

// Wrong argument types
var_dump( lchown( new StdClass(), $uid ) );
var_dump( lchown( array(), $uid ) );

// Bad user
var_dump( lchown( $filename, -5 ) );

?>
===DONE===
--CLEAN--
<?php

$filename = __DIR__ . DIRECTORY_SEPARATOR . 'lchown.txt';
unlink($filename);

?>
--EXPECTF--
*** Testing lchown() : error functionality ***

Warning: lchown() expects exactly 2 parameters, 1 given in %s on line %d
bool(true)

Warning: lchown() expects exactly 2 parameters, 3 given in %s on line %d
bool(true)

Warning: lchown(): No such file or directory in %s on line %d
bool(false)

Warning: lchown() expects parameter 1 to be a valid path, object given in %s on line %d
bool(true)

Warning: lchown() expects parameter 1 to be a valid path, array given in %s on line %d
bool(true)

Warning: lchown(): %r(Operation not permitted|Invalid argument)%r in %s on line %d
bool(false)
===DONE===
