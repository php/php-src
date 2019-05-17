--TEST--
PDO_sqlite: Testing PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES
--SKIPIF--
<?php if (!extension_loaded('pdo_sqlite')) print 'skip not loaded'; ?>
--FILE--
<?php

echo "Creating new PDO" . PHP_EOL;
$db = new PDO('sqlite::memory:');

$db->exec("CREATE TABLE dog ( id INTEGER PRIMARY KEY, refid INTEGER, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 687971, 'Annoying Dog', 1)");
echo "First Error Info:" . PHP_EOL;
print_r($db->errorInfo());

echo "Inserting second time which should fail" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 687971, 'Annoying Dog', 1)");
echo "Second Error Info:" . PHP_EOL;
print_r($db->errorInfo());

echo "Creating new PDO with Extended Result Codes turned on" . PHP_EOL;
$db = new PDO('sqlite::memory:', '', '', [PDO::PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES => 1]);

$db->exec("CREATE TABLE dog ( id INTEGER PRIMARY KEY, refid INTEGER, name TEXT, annoying INTEGER )");

echo "Inserting first time which should succeed" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 687971, 'Annoying Dog', 1)");
echo "First Error Info:" . PHP_EOL;
print_r($db->errorInfo());

echo "Inserting second time which should fail" . PHP_EOL;
$result = $db->exec("INSERT INTO dog VALUES (1, 687971, 'Annoying Dog', 1)");
echo "Second (Extended) Error Info:" . PHP_EOL;
print_r($db->errorInfo());

?>
--EXPECTF--
Creating new PDO
Inserting first time which should succeed
First Error Info:
Array
(
    [0] => 00000
    [1] =>
    [2] =>
)
Inserting second time which should fail
Second Error Info:
Array
(
    [0] => 23000
    [1] => 19
    [2] => PRIMARY KEY must be unique
)
Creating new PDO with Extended Result Codes turned on
Inserting first time which should succeed
First Error Info:
Array
(
    [0] => 00000
    [1] =>
    [2] =>
)
Inserting second time which should fail
Second (Extended) Error Info:
Array
(
    [0] => HY000
    [1] => 1555
    [2] => PRIMARY KEY must be unique
)
