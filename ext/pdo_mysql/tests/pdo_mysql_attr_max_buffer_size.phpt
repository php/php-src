--TEST--
MySQL PDO->__construct(), PDO::MYSQL_ATTR_MAX_BUFFER_SIZE
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
if (MySQLPDOTest::isPDOMySQLnd())
    die("skip PDO::MYSQL_ATTR_MAX_BUFFER_SIZE not supported with mysqlnd");
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

function try_buffer_size($offset, $buffer_size)
{
    /* unsigned overflow possible ? */
    $db = new PDO(MySQLPDOTest::getDSN(), PDO_MYSQL_TEST_USER, PDO_MYSQL_TEST_PASS, [
        PDO::MYSQL_ATTR_MAX_BUFFER_SIZE => $buffer_size,
        /* buffer is only relevant with native PS */
        PDO::MYSQL_ATTR_DIRECT_QUERY => 0,
        PDO::ATTR_EMULATE_PREPARES => 0,
    ]);

    $db->exec(sprintf('CREATE TABLE test_attr_max_buffer_size(id INT, val LONGBLOB) ENGINE = %s', PDO_MYSQL_TEST_ENGINE));

    // 10 * (10 * 1024) = 10 * (10 * 1k) = 100k
    $db->exec('INSERT INTO test_attr_max_buffer_size(id, val) VALUES (1, REPEAT("01234567890", 10240))');

    $stmt = $db->prepare('SELECT id, val FROM test_attr_max_buffer_size');
    $stmt->execute();

    $stmt->bindColumn(1, $id);
    $stmt->bindColumn(2, $val);
    while ($stmt->fetch(PDO::FETCH_BOUND)) {
        printf(
            "[%03d] id = %d, val = %s... (length: %d)\n",
            $offset,
            $id,
            substr($val, 0, 10),
            strlen($val)
        );
    }

    // This is necessary because we will be creating tables several times.
    $db->query('DROP TABLE test_attr_max_buffer_size');
}

try_buffer_size(1, -1);
try_buffer_size(2, 1000);
try_buffer_size(4, 2000);
try {
    try_buffer_size(3, null);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->query('DROP TABLE IF EXISTS test_attr_max_buffer_size');
?>
--EXPECTF--
[001] id = 1, val = 0123456789... (length: %d)
[002] id = 1, val = 0123456789... (length: 1000)
[004] id = 1, val = 0123456789... (length: 2000)
Attribute value must be of type int for selected attribute, null given
done!
