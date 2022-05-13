--TEST--
PDO_sqlite: Testing PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

echo "Creating new PDO" . PHP_EOL;
$db = new PDO('sqlite::memory:');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec("CREATE TABLE dog ( id INTEGER PRIMARY KEY, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;
$db->exec("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
$errorInfo = $db->errorInfo();
echo sprintf("First Error Info: SQLSTATE Error Code: (%s), Driver Specific Error Code: (%s)", $errorInfo[0], $errorInfo[1]) . PHP_EOL;

echo "Inserting second time which should fail" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
$errorInfo = $db->errorInfo();
echo sprintf("Second Error Info: SQLSTATE Error Code: (%s), Driver Specific Error Code: (%s)", $errorInfo[0], $errorInfo[1]) . PHP_EOL;


echo "Creating new PDO with Extended Result Codes turned on" . PHP_EOL;
$db = new PDO('sqlite::memory:', '', '', [PDO::SQLITE_ATTR_EXTENDED_RESULT_CODES => TRUE]);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec("CREATE TABLE dog ( id INTEGER PRIMARY KEY, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
$errorInfo = $db->errorInfo();
echo sprintf("First (Extended) Error Info: SQLSTATE Error Code: (%s), Driver Specific Error Code: (%s)", $errorInfo[0], $errorInfo[1]) . PHP_EOL;

echo "Inserting second time which should fail" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 'Annoying Dog', 1)");
$errorInfo = $db->errorInfo();
echo sprintf("Second (Extended) Error Info: SQLSTATE Error Code: (%s), Driver Specific Error Code: (%s)", $errorInfo[0], $errorInfo[1]) . PHP_EOL;

?>
--EXPECT--
Creating new PDO
Inserting first time which should succeed
First Error Info: SQLSTATE Error Code: (00000), Driver Specific Error Code: ()
Inserting second time which should fail
Second Error Info: SQLSTATE Error Code: (23000), Driver Specific Error Code: (19)
Creating new PDO with Extended Result Codes turned on
Inserting first time which should succeed
First (Extended) Error Info: SQLSTATE Error Code: (00000), Driver Specific Error Code: ()
Inserting second time which should fail
Second (Extended) Error Info: SQLSTATE Error Code: (HY000), Driver Specific Error Code: (1555)
