--TEST--
PDO_DBLIB bug #68957 PDO::query doesn't support several queries
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();

$query = "declare @myInt int = 1; select @myInt;";
$stmt = $db->query($query);
$stmt->nextRowset(); // Added line
$rows = $stmt->fetchAll();
print_r($rows);

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [computed] => 1
            [0] => 1
        )

)
