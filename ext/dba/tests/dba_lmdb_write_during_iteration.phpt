--TEST--
DBA LMDB: writes during cursor iteration end the iteration cleanly
--EXTENSIONS--
dba
--SKIPIF--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
check_skip('lmdb');
?>
--FILE--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_file = __DIR__ . '/dba_lmdb_write_during_iteration.db';
@unlink($db_file);
@unlink($db_file . '-lock');

$db = dba_open($db_file, 'cl', 'lmdb');
foreach (['a' => '1', 'b' => '2', 'c' => '3'] as $k => $v) {
    dba_replace($k, $v, $db);
}

var_dump(dba_firstkey($db));
var_dump(dba_nextkey($db));

var_dump(dba_replace('d', '4', $db));
var_dump(dba_nextkey($db));
var_dump(dba_nextkey($db));

var_dump(dba_firstkey($db));
var_dump(dba_delete('a', $db));
var_dump(dba_nextkey($db));

dba_close($db);
?>
--CLEAN--
<?php
$db_file = __DIR__ . '/dba_lmdb_write_during_iteration.db';
@unlink($db_file);
@unlink($db_file . '-lock');
@unlink($db_file . '.lck');
?>
--EXPECTF--
Notice: dba_open(): Handler lmdb does locking internally in %s on line %d
string(1) "a"
string(1) "b"
bool(true)
bool(false)
bool(false)
string(1) "a"
bool(true)
bool(false)
