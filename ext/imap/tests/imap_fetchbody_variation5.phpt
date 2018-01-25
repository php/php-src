--TEST--
Test imap_fetchbody() function : usage variation - different resources as $stream_id arg
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchbody(resource $stream_id, int $msg_no, string $section
 *           [, int options])
 * Description: Get a specific body section
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different resource types to imap_fetchbody() to test behaviour
 */

echo "*** Testing imap_fetchbody() : usage variations ***\n";

echo "\n-- File Resource opened with fopen() --\n";
var_dump($file_pointer = fopen(__FILE__, 'r+'));
var_dump(imap_fetchbody($file_pointer, 1));
fclose($file_pointer);

echo "\n-- Directory Resource opened with opendir() --\n";
var_dump($dir_handle = opendir(dirname(__FILE__)));
var_dump(imap_fetchbody($dir_handle, 1));
closedir($dir_handle);
?>
===DONE===
--EXPECTF--
*** Testing imap_fetchbody() : usage variations ***

-- File Resource opened with fopen() --
resource(5) of type (stream)

Warning: imap_fetchbody() expects at least 3 parameters, 2 given in %s on line %d
NULL

-- Directory Resource opened with opendir() --
resource(6) of type (stream)

Warning: imap_fetchbody() expects at least 3 parameters, 2 given in %s on line %d
NULL
===DONE===
