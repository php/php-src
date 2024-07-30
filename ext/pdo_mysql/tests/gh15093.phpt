--TEST--
GH-15093: Add support for all flags from mysql in the PDO MySql driver in the getColumnMeta function.
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skipNotMySQLnd();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();

$db->exec("
    CREATE TABLE `gh_15093` (
        `id` INT NOT NULL AUTO_INCREMENT,
        `uuid` BINARY(16) DEFAULT (uuid_to_bin(uuid())),
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
Array
(
    [0] => Array
        (
            [native_type] => LONG
            [flags] => Array
                (
                    [0] => not_null
                    [1] => primary_key
                    [2] => auto_increment
                    [3] => num
                )

            [native_flags] => 49667
            [table] => gh_15093
            [name] => id
            [len] => 11
            [precision] => 0
        )

    [1] => Array
        (
            [native_type] => STRING
            [flags] => Array
                (
                    [0] => unique_key
                    [1] => binary
                )

            [native_flags] => 16516
            [table] => gh_15093
            [name] => uuid
            [len] => 16
            [precision] => 0
        )

    [2] => Array
        (
            [native_type] => BLOB
            [flags] => Array
                (
                    [0] => blob
                    [1] => binary
                )

            [native_flags] => 144
            [table] => gh_15093
            [name] => blob
            [len] => 65535
            [precision] => 0
        )

    [3] => Array
        (
            [native_type] => TIMESTAMP
            [flags] => Array
                (
                    [0] => binary
                    [1] => timestamp
                    [2] => on_update_now
                )

            [native_flags] => 9344
            [table] => gh_15093
            [name] => ts
            [len] => 19
            [precision] => 0
        )

    [4] => Array
        (
            [native_type] => STRING
            [flags] => Array
                (
                    [0] => set
                )

            [native_flags] => 2048
            [table] => gh_15093
            [name] => set
            [len] => 28
            [precision] => 0
        )

    [5] => Array
        (
            [native_type] => STRING
            [flags] => Array
                (
                    [0] => enum
                )

            [native_flags] => 256
            [table] => gh_15093
            [name] => enum
            [len] => 4
            [precision] => 0
        )

    [6] => Array
        (
            [native_type] => LONG
            [flags] => Array
                (
                    [0] => unsigned
                    [1] => zerofill
                    [2] => num
                )

            [native_flags] => 32864
            [table] => gh_15093
            [name] => num
            [len] => 11
            [precision] => 0
        )

)
