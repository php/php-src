--TEST--
DBA LMDB: dba_nextkey before dba_firstkey returns false
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
$db_file = __DIR__ . '/dba_lmdb_nextkey_without_firstkey.db';
@unlink($db_file);
@unlink($db_file . '-lock');

$db = dba_open($db_file, 'cl', 'lmdb');
var_dump(dba_nextkey($db));
dba_replace('a', '1', $db);
var_dump(dba_nextkey($db));
dba_close($db);
?>
--CLEAN--
<?php
$db_file = __DIR__ . '/dba_lmdb_nextkey_without_firstkey.db';
@unlink($db_file);
@unlink($db_file . '-lock');
@unlink($db_file . '.lck');
?>
--EXPECTF--
Notice: dba_open(): Handler lmdb does locking internally in %s on line %d
bool(false)
bool(false)
