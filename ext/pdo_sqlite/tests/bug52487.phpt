--TEST--
Bug #52487 (PDO::FETCH_INTO leaks memory)
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded('pdo_sqlite')) print 'skip not loaded';
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');

$stmt = $db->prepare("select 1 as attr");
for ($i = 0; $i < 10; $i++) {
	$stmt->setFetchMode(PDO::FETCH_INTO, new stdClass);
}

print "ok\n";

?>
--EXPECT--
ok
