--TEST--
DBA LMDB handler readonly test
--EXTENSIONS--
dba
--SKIPIF--
<?php
    $handler = 'lmdb';
    require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
$handler = 'lmdb';
$db_filename = __DIR__ . "/lmdb-readonly.dbm";

// Create DB
$db_file = dba_open($db_filename, "c", $handler);
assert($db_file !== false);
// Close handler
dba_close($db_file);

// Open in read only mode
$db_file = dba_open($db_filename, "r", $handler);
assert($db_file !== false);

// Try inserting
dba_insert("key1", "This is a test insert", $db_file);
dba_close($db_file);
?>
--CLEAN--
<?php
$db_filename = __DIR__ . "/lmdb-readonly.dbm";
@unlink($db_filename);
@unlink($db_filename.'.lck');
@unlink($db_filename.'-lock');
?>
--EXPECTF--
Warning: dba_insert(): Cannot perform a modification on a readonly database in %s on line %d
