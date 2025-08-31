--TEST--
Bug #78808 ([LMDB] MDB_MAP_FULL: Environment mapsize limit reached)
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
$db_name = 'bug78808.db';

$value = str_repeat('*', 0x100000);
$lmdb_h = dba_open($db_name, 'c', 'lmdb', 0644, 5*1048576);
for ($i = 0; $i < 3; $i++) {
    dba_insert('key' . $i, $value, $lmdb_h);
}
dba_close($lmdb_h);
echo "done\n";
?>
--EXPECT--
done
--CLEAN--
<?php
require_once __DIR__ . '/setup/setup_dba_tests.inc';
$db_name = 'bug78808.db';
cleanup_standard_db($db_name);
?>
