--TEST--
finfo_open(): Testing magic_file names
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

var_dump(finfo_open(FILEINFO_MIME, "\0"));
var_dump(finfo_open(FILEINFO_MIME, NULL));
var_dump(finfo_open(FILEINFO_MIME, ''));
var_dump(finfo_open(FILEINFO_MIME, 123));
var_dump(finfo_open(FILEINFO_MIME, 1.0));
var_dump(finfo_open(FILEINFO_MIME, '/foo/bar/inexistent'));

?>
--EXPECTF--
Warning: finfo_open() expects parameter 2 to be a valid path, string given in %s on line %d
bool(false)
resource(%d) of type (file_info)
resource(%d) of type (file_info)

Warning: finfo_open(%s123): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%s123): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at '%s123'. in %s on line %d
bool(false)

Warning: finfo_open(%s1): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%s1): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at '%s1'. in %s on line %d
bool(false)

Warning: finfo_open(%sinexistent): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(%sinexistent): failed to open stream: No such file or directory in %s on line %d

Warning: finfo_open(): Failed to load magic database at '%sinexistent'. in %s on line %d
bool(false)
