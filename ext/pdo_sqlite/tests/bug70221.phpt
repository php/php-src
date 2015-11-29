--TEST--
Bug #70221 (persistent sqlite connection + custom function segfaults)
--SKIPIF--
<?php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
$db = new PDO('sqlite:test.sqlite', null, null, array(PDO::ATTR_PERSISTENT => true));
function _test() { return 42; }
$db->sqliteCreateFunction('test', '_test', 0);
print("Everything is fine, no exceptions here\n");
?>
--EXPECT--
Everything is fine, no exceptions here
