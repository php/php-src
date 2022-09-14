--TEST--
Bug GH-9372 (HY010 when binding overlong parameter)
--SKIPIF--
<?php
if (!extension_loaded('pdo_odbc')) die('skip pdo_odbc extension not available');
require 'ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec("CREATE TABLE gh9372 (col VARCHAR(10))");
$db->exec("INSERT INTO gh9372 VALUES ('something')");

$stmt = $db->prepare("SELECT * FROM bug74186 WHERE col = ?");
$stmt->bindValue(1, 'something else');
try {
    $stmt->execute();
} catch (PDOException $ex) {
    if ($ex->getCode() !== "22001") {
        var_dump($ex->getMessage());
    }
}

$stmt = $db->prepare("SELECT * FROM bug74186 WHERE col = ?");
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
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->exec("DROP TABLE gh9372");
?>
--EXPECT--
