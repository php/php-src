--TEST--
compress.bzip2:// opener must not unlink a NULL opened_path when the open fails
--EXTENSIONS--
bz2
dba
--SKIPIF--
<?php
if (!in_array('flatfile', dba_handlers())) die('skip flatfile handler not available');
?>
--FILE--
<?php
var_dump(dba_open("compress.bzip2://" . __DIR__ . "/nonexistent-dir/x", "nd", "flatfile"));
?>
--EXPECTF--
Warning: dba_open(%S): Failed to open stream: operation failed in %s on line %d
bool(false)
