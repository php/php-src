--TEST--
DBA CDB handler test
--SKIPIF--
<?php
$handler = 'cdb';
require_once(__DIR__ .'/skipif.inc');
die('info CDB does not support replace or delete');
?>
--FILE--
<?php

$handler = 'cdb';
require_once(__DIR__ .'/test.inc');

echo "Test 0\n";

if (($db_file = dba_open($db_filename, 'n', $handler))!==FALSE) {
    var_dump(dba_insert("key1", "Content String 1", $db_file));
    var_dump(dba_replace("key1", "New Content String", $db_file));
    var_dump(dba_fetch("key1", $db_file));
    var_dump(dba_firstkey($db_file));
    var_dump(dba_delete("key1", $db_file));
    var_dump(dba_optimize($db_file));
    var_dump(dba_sync($db_file));
    dba_close($db_file);
}
else {
    echo "Failed to open DB\n";
}

unlink($db_filename);

echo "Test 1\n";

if (($db_file = dba_open($db_filename, 'c', $handler))!==FALSE) {
    dba_insert("key1", "Content String 1", $db_file);
    dba_close($db_file);
}
else {
    echo "Failed to open DB\n";
}

echo "Test 2\n";

if (($db_file = dba_open($db_filename, 'r', $handler))!==FALSE) {
    dba_insert("key1", "Content String 1", $db_file);
    dba_close($db_file);
}
else {
    echo "Failed to open DB\n";
}

echo "Test 3\n";

if (($db_file = dba_open($db_filename, 'w', $handler))!==FALSE) {
    echo dba_fetch("key1", $db_file), "\n";
    dba_close($db_file);
}
else {
    echo "Failed to open DB\n";
}

?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
Test 0
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
Test 1

Warning: dba_open(%stest0.dbm,c): Driver initialization failed for handler: cdb: Update operations are not supported in %sdba_cdb_001.php on line %d
Failed to open DB
Test 2

Warning: dba_insert(): You cannot perform a modification to a database without proper access in %sdba_cdb_001.php on line %d
Test 3

Warning: dba_open(%stest0.dbm,w): Driver initialization failed for handler: cdb: Update operations are not supported in %sdba_cdb_001.php on line %d
Failed to open DB
