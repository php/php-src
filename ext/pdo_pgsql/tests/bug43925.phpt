--TEST--
Bug #43925 (Incorrect argument counter in prepared statements with pgsql)
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
$dbh = PDOTest::test_factory(__DIR__ . '/common.phpt');

$dbh->query('
CREATE TABLE test43925
(
   id   integer NOT NULL PRIMARY KEY
 , root integer NOT NULL
 , lft  integer NOT NULL
 , rgt  integer NOT NULL
);');

$dbh->query('INSERT INTO test43925 (id, root, lft, rgt) VALUES (1, 1, 1, 6), (2, 1, 2, 3), (3, 1, 4, 5);');


$stmt = $dbh->prepare('
    SELECT *
    FROM test43925
    WHERE (:rootId > 0 OR lft > :left OR rgt > :left)
        AND (root = :rootId OR root  = :left)
        AND (1 > :left OR 1 < :left OR 1 = :left)
        AND (:x > 0 OR :x < 10 OR :x > 100)
        OR :y = 1 OR :left = 1
');

$stmt->bindValue('left',   1, PDO::PARAM_INT);
$stmt->bindValue('rootId', 3, PDO::PARAM_INT);
$stmt->bindValue('x', 5, PDO::PARAM_INT);
$stmt->bindValue('y', 50, PDO::PARAM_INT);

$stmt->execute();

foreach ($stmt->fetchAll() as $row) {
    print implode(' - ', $row);
    print "\n";
}
?>
--CLEAN--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->exec("DROP TABLE IF EXIST test43925");
?>
--EXPECT--
1 - 1 - 1 - 1 - 1 - 1 - 6 - 6
2 - 2 - 1 - 1 - 2 - 2 - 3 - 3
3 - 3 - 1 - 1 - 4 - 4 - 5 - 5
