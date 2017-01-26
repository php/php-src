--TEST--
PDO_DBLIB bug #68957 PDO::query doesn't support several queries
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

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

