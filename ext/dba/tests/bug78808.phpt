--TEST--
Bug #78808 ([LMDB] MDB_MAP_FULL: Environment mapsize limit reached)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
$handler = 'lmdb';
require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
$handler = 'lmdb';
require_once __DIR__ .'/test.inc';
$lmdb_h = dba_open($db_filename, 'c', 'lmdb', 0644, 5*1048576);
for ($i = 0; $i < 50000; $i++) {
    dba_insert('key' . $i, 'value '. $i, $lmdb_h);
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
