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

@$dbh->query('DROP TABLE nodes');

$dbh->query('
CREATE TABLE nodes
(
   id   integer NOT NULL PRIMARY KEY
 , root integer NOT NULL
 , lft  integer NOT NULL
 , rgt  integer NOT NULL
);');

$dbh->query('INSERT INTO nodes (id, root, lft, rgt) VALUES (1, 1, 1, 6);');
$dbh->query('INSERT INTO nodes (id, root, lft, rgt) VALUES (2, 1, 2, 3);');
$dbh->query('INSERT INTO nodes (id, root, lft, rgt) VALUES (3, 1, 4, 5);');


$stmt = $dbh->prepare('
    SELECT *
    FROM nodes
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

$dbh->query('DROP TABLE nodes');

?>
--EXPECT--
1 - 1 - 1 - 1 - 1 - 1 - 6 - 6
2 - 2 - 1 - 1 - 2 - 2 - 3 - 3
3 - 3 - 1 - 1 - 4 - 4 - 5 - 5
