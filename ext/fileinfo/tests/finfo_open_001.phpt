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
Warning: finfo_open(): Failed to load magic database at ''. in %s on line %d
bool(false)
resource(%d) of type (file_info)
resource(%d) of type (file_info)
bool(false)
bool(false)
bool(false)
