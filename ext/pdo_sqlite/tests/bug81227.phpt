--TEST--
Bug #81227 (PDO::inTransaction reports false when in transaction)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO("sqlite::memory:");
var_dump($db->inTransaction());

$db->exec("BEGIN EXCLUSIVE TRANSACTION");
var_dump($db->inTransaction());

try {
    $db->beginTransaction();
} catch (PDOException $e) {
    echo $e->getMessage(), "\n";
}

$db->commit();
var_dump($db->inTransaction());

$db->beginTransaction();
var_dump($db->inTransaction());
?>
--EXPECT--
bool(false)
bool(true)
There is already an active transaction
bool(false)
bool(true)
