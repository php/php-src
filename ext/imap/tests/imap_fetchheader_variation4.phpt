--TEST--
Test imap_fetchheader() function : usage variations - diff resource types as $stream_id
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchheader(resource $stream_id, int $msg_no [, int $options])
 * Description: Get the full unfiltered header for a message
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different types of resources to imap_fetchheader() to test behaviour
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

echo "\n-- File Resource opened with fopen() --\n";
var_dump($file_pointer = fopen(__FILE__, 'r+'));
var_dump(imap_fetchheader($file_pointer, 1));
fclose($file_pointer);

echo "\n-- Directory Resource opened with opendir() --\n";
var_dump($dir_handle = opendir(dirname(__FILE__)));
var_dump(imap_fetchheader($dir_handle, 1));
closedir($dir_handle);
?>
===DONE===
--EXPECTF--
*** Testing imap_fetchheader() : usage variations ***

-- File Resource opened with fopen() --
resource(%d) of type (stream)

Warning: imap_fetchheader(): supplied resource is not a valid imap resource in %s on line %d
bool(false)

-- Directory Resource opened with opendir() --
resource(%d) of type (stream)

Warning: imap_fetchheader(): supplied resource is not a valid imap resource in %s on line %d
bool(false)
===DONE===
