--TEST--
GH-15093: Add support for all flags from mysql in the PDO MySql driver in the getColumnMeta function.
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->exec("
    CREATE TABLE `gh_15093` (
        `id` INT NOT NULL AUTO_INCREMENT,
        `uuid` BINARY(16),
        `blob` BLOB,
        `ts` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
        `set` SET('one', 'two'),
        `enum` ENUM('a', 'b', 'c'),
        `num` INT(11) UNSIGNED ZEROFILL DEFAULT 0,
        PRIMARY KEY(`id`),
        UNIQUE KEY `UUID` (`uuid`)
    )
");

$stmt = $db->prepare('SELECT `id`, `uuid`, `blob`, `ts`, `set`, `enum`, `num` FROM gh_15093');
$stmt->execute();

$n = $stmt->columnCount();
$meta = [];

for ($i = 0; $i < $n; ++$i) {
    $m = $stmt->getColumnMeta($i);

    // libmysql and mysqlnd will show the pdo_type entry at a different position in the hash
    // and will report a different type, as mysqlnd returns native types.
    unset($m['pdo_type']);

    $meta[$i] = $m;
}

print_r($meta);
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS gh_15093');
?>
--EXPECTF--
