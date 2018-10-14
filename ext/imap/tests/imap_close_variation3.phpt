--TEST--
Test imap_close() function : usage variations - different streams
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : bool imap_close(resource $stream_id [, int $options])
 * Description: Close an IMAP stream
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different stream types to imap_close() to test whether it can close them
 */

echo "*** Testing imap_close() : usage variations ***\n";

echo "\n-- File Resource opened with fopen() --\n";
var_dump($file_handle = fopen(__FILE__, 'r'));
var_dump(imap_close($file_handle));
var_dump($file_handle);

echo "\n-- Directory Resource opened with opendir() --\n";
var_dump($dir_handle = opendir(dirname(__FILE__)));
var_dump(imap_close($dir_handle));
var_dump($dir_handle);
?>
===DONE===
--EXPECTF--
*** Testing imap_close() : usage variations ***

-- File Resource opened with fopen() --
resource(%d) of type (stream)

Warning: imap_close(): supplied resource is not a valid imap resource in %s on line %d
bool(false)
resource(%d) of type (stream)

-- Directory Resource opened with opendir() --
resource(%d) of type (stream)

Warning: imap_close(): supplied resource is not a valid imap resource in %s on line %d
bool(false)
resource(%d) of type (stream)
===DONE===
