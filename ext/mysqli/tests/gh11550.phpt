--TEST--
Bug GH-11550 (MySQL Statement has a empty query result when the response field has changed, also Segmentation fault)
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php
require_once 'connect.inc';

$link = new \mysqli($host, $user, $passwd, $db, $port, $socket);

$link->query(<<<'SQL'
CREATE TABLE `test_gh11550`  (
    `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
    `name` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL,
    PRIMARY KEY (`id`) USING BTREE,
    INDEX `name`(`name`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = Dynamic;
SQL);
$link->query(<<<'SQL'
INSERT INTO `test_gh11550` (`name`) VALUES ('test1');
SQL);

$stmt = $link->prepare('select * from test_gh11550');
var_dump('mysqli-1:', $stmt->execute(), $stmt->get_result()->fetch_all());

$link->query(<<<'SQL'
ALTER TABLE `test_gh11550`
ADD COLUMN `a` varchar(255) NOT NULL DEFAULT '';
SQL);

var_dump('mysqli-2:', $stmt->execute(), $stmt->get_result()->fetch_all());
echo 'Done';
?>
--CLEAN--
<?php
require_once 'connect.inc';

$link = new \mysqli($host, $user, $passwd, $db, $port, $socket);
$link->query('DROP TABLE IF EXISTS test_gh11550');
?>
--EXPECT--
string(9) "mysqli-1:"
bool(true)
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    string(5) "test1"
  }
}
string(9) "mysqli-2:"
bool(true)
array(1) {
  [0]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    string(5) "test1"
    [2]=>
    string(0) ""
  }
}
Done
