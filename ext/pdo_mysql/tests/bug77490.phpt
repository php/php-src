--TEST--
Bug #77490 (PDO mysql does not give exception for too many params when none required)
--SKIPIF--
<?php
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'skipif.inc');
require_once(dirname(__FILE__) . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();

?>
--FILE--
<?php

include __DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc';

// Test exception mode throws
echo "Testing exception.\n";
$db = MySQLPDOTest::factory();

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$createSql = "CREATE TABLE `bug77490` (
  `label` varchar(255) NOT NULL DEFAULT '0'
)";

$db->exec('drop table if exists bug77490');
$db->exec($createSql);
$insertSql = "insert into bug77490 (label) values (label)";
$stmt = $db->prepare($insertSql);
$values = [
    ':label' => 'foo',
];

try {
    $stmt->execute($values);
    echo "Failed to throw exception\n";
}
catch (PDOException $pdoException) {
    echo "Ok, exception caught: " . $pdoException->getMessage() . "\n";
}


// Test non-exception mode gives warning
echo "Testing warning.";
$db = MySQLPDOTest::factory();

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

$createSql = "CREATE TABLE `bug77490` (
  `label` varchar(255) NOT NULL DEFAULT '0'
)";

$db->exec('drop table if exists bug77490');
$db->exec($createSql);
$insertSql = "insert into bug77490 (label) values (label)";
$stmt = $db->prepare($insertSql);
$values = [
    ':label' => 'foo',
];

try {
    $stmt->execute($values);
}
catch (PDOException $pdoException) {
    echo "fail, exception shouldn't have been thrown: " . $pdoException->getMessage() . "\n";
}

echo "done\n";

?>
--EXPECTF--
Testing exception.
Ok, exception caught: SQLSTATE[HY093]: Invalid parameter number
Testing warning.
Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number in %s on line %d
done
