--TEST--
Bug GH-9372 (HY010 when binding overlong parameter)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
ODBCPDOTest::skip();
?>
--FILE--
<?php
// Executing the statements fails with some drivers, but not others.
// The test is written in a way to always pass, unless the execution
// fails with a different code than 22001 (String data, right truncation).

require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$table_name = 'gh9372_pdo_odbc';

$db->exec("CREATE TABLE {$table_name} (col VARCHAR(10))");
$db->exec("INSERT INTO {$table_name} VALUES ('something')");

$stmt = $db->prepare("SELECT * FROM {$table_name} WHERE col = ?");
$stmt->bindValue(1, 'something else');
try {
    $stmt->execute();
} catch (PDOException $ex) {
    if ($ex->getCode() !== "22001") {
        var_dump($ex->getMessage());
    }
}

$stmt = $db->prepare("SELECT * FROM {$table_name} WHERE col = ?");
$stream = fopen("php://memory", "w+");
fwrite($stream, 'something else');
rewind($stream);
$stmt->bindvalue(1, $stream, PDO::PARAM_LOB);
try {
    $stmt->execute();
} catch (PDOException $ex) {
    if ($ex->getCode() !== "22001") {
        var_dump($ex->getMessage());
    }
}
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$db = ODBCPDOTest::factory();
$db->exec("DROP TABLE IF EXISTS gh9372_pdo_odbc");
?>
--EXPECT--
