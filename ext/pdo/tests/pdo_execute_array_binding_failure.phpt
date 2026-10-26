--TEST--
PDO: execute() leaves no bindings when binding its array argument fails
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

class ThrowingString
{
    public function __toString(): string
    {
        throw new RuntimeException('conversion failed');
    }
}

function bound_params_count(PDOStatement $stmt): string
{
    ob_start();
    $stmt->debugDumpParams();
    preg_match('/^Params:\s+(\d+)$/m', ob_get_clean(), $m);
    return $m[1];
}

$db = PDOTest::factory();
$db->exec('CREATE TABLE test_execute_bind_fail (id int, name varchar(10))');
$db->exec("INSERT INTO test_execute_bind_fail (id, name) VALUES (1, 'a')");
$db->exec("INSERT INTO test_execute_bind_fail (id, name) VALUES (2, 'b')");

$stmt = $db->prepare('SELECT name FROM test_execute_bind_fail WHERE id = :id AND name = :name');
$id = 1;
$name = 'a';
$stmt->bindParam(':id', $id);
$stmt->bindParam(':name', $name);

try {
    $stmt->execute([':id' => 2, ':name' => new ThrowingString()]);
} catch (RuntimeException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
echo "bound params after failure: ", bound_params_count($stmt), PHP_EOL;

var_dump($stmt->execute([':id' => 2, ':name' => 'b']));
var_dump($stmt->fetchAll(PDO::FETCH_COLUMN));
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'test_execute_bind_fail');
?>
--EXPECT--
RuntimeException: conversion failed
bound params after failure: 0
bool(true)
array(1) {
  [0]=>
  string(1) "b"
}
