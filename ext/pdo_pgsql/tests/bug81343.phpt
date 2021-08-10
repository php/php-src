--TEST--
Bug #81343 pdo_pgsql: Inconsitent boolean conversion after calling closeCursor()
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);
$sth = $pdo->prepare("select false where 2=?");

for ($i = 0; $i < 2; $i++) {
    $sth->execute([2]);
    var_dump($sth->fetchColumn(0));
    $sth->closeCursor();
}
?>
--EXPECT--
bool(false)
bool(false)
