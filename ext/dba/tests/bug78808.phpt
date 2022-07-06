--TEST--
Bug #78808 ([LMDB] MDB_MAP_FULL: Environment mapsize limit reached)
--SKIPIF--
<?php
$handler = 'lmdb';
require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
$handler = 'lmdb';
require_once __DIR__ .'/test.inc';
$value = str_repeat('*', 0x100000);
$lmdb_h = dba_open($db_filename, 'c', 'lmdb', 0644, 5*1048576);
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
require_once dirname(__FILE__) .'/clean.inc';
?>
