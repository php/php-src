--TEST--
PDO ODBC varying character with max/no length
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
ODBCPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$table_name = 'test_max_columns_pdo_odbc';

if (false === $db->exec("CREATE TABLE {$table_name} (id INT NOT NULL PRIMARY KEY, data varchar(max))")) {
    if (false === $db->exec("CREATE TABLE {$table_name} (id INT NOT NULL PRIMARY KEY, data longtext)")) {
        if (false === $db->exec("CREATE TABLE {$table_name} (id INT NOT NULL PRIMARY KEY, data CLOB)")) {
            die("BORK: don't know how to create a long column here:\n" . implode(", ", $db->errorInfo()));
        }
    }
}

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$sizes = array(32, 64, 128, 253, 254, 255, 256, 257, 258, 512, 1024, 2048, 3998, 3999, 4000);

$db->beginTransaction();
$insert = $db->prepare("INSERT INTO {$table_name} VALUES (?, ?)");
foreach ($sizes as $num) {
    $insert->execute(array($num, str_repeat('i', $num)));
}
$insert = null;
$db->commit();

foreach ($db->query("SELECT id, data FROM {$table_name}") as $row) {
    $expect = str_repeat('i', $row[0]);
    if (strcmp($expect, $row[1])) {
        echo "Failed on size $row[id]:\n";
        printf("Expected %d bytes, got %d\n", strlen($expect), strlen($row['data']));
        echo bin2hex($expect) . "\n";
        echo bin2hex($row['data']) . "\n";
    }
}

echo "Finished\n";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS test_max_columns_pdo_odbc");
?>
--EXPECT--
Finished
