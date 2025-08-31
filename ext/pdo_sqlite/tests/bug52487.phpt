--TEST--
Bug #52487 (PDO::FETCH_INTO leaks memory)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');

$stmt = $db->prepare("select 1 as attr");
for ($i = 0; $i < 10; $i++) {
    $stmt->setFetchMode(PDO::FETCH_INTO, new stdClass);
}

print "ok\n";

?>
--EXPECT--
ok
