--TEST--
Bug #70221 (persistent sqlite connection + custom function segfaults)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$dbfile = __DIR__ . '/test.sqlite';
$db = new PDO('sqlite:'.$dbfile, null, null, array(PDO::ATTR_PERSISTENT => true));
function _test() { return 42; }
$db->sqliteCreateFunction('test', '_test', 0);
print("Everything is fine, no exceptions here\n");
unset($db);
?>
--CLEAN--
<?php
$dbfile = __DIR__ . '/test.sqlite';
unlink($dbfile);
?>
--EXPECT--
Everything is fine, no exceptions here
