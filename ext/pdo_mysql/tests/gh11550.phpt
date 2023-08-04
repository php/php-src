--TEST--
Bug GH-11550 (MySQL Statement has a empty query result when the response field has changed, also Segmentation fault)
--EXTENSIONS--
pdo
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();

$pdo->exec(<<<'SQL'
CREATE TABLE `test_gh11550`  (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `name`(`name`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;
SQL);
$pdo->exec(<<<'SQL'
INSERT INTO `test_gh11550` (`name`) VALUES ('test1');
SQL);

$stmt = $pdo->prepare('select * from test_gh11550');
var_dump('PDO-1:', $stmt->execute(), $stmt->fetchAll());

$stmt->closeCursor(); // Optional. Segmentation fault (core dumped)

$pdo->exec(<<<'SQL'
ALTER TABLE `test_gh11550`
ADD COLUMN `a` varchar(255) NOT NULL DEFAULT '';
SQL);

var_dump('PDO-2:', $stmt->execute(), $stmt->fetchAll());
echo 'Done';
?>
--CLEAN--
<?php
require_once __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';
$pdo = MySQLPDOTest::factory();
$pdo->query('DROP TABLE IF EXISTS test_gh11550');
?>
--EXPECT--
string(6) "PDO-1:"
bool(true)
array(1) {
  [0]=>
  array(4) {
    ["id"]=>
    int(1)
    [0]=>
    int(1)
    ["name"]=>
    string(5) "test1"
    [1]=>
    string(5) "test1"
  }
}
string(6) "PDO-2:"
bool(true)
array(1) {
  [0]=>
  array(6) {
    ["id"]=>
    int(1)
    [0]=>
    int(1)
    ["name"]=>
    string(5) "test1"
    [1]=>
    string(5) "test1"
    ["a"]=>
    string(0) ""
    [2]=>
    string(0) ""
  }
}
Done
