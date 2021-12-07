--TEST--
PDO Common: Bug #36798 (Error parsing named parameters with queries containing high-ascii chars)
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

if (!strncasecmp(getenv('PDOTEST_DSN'), 'oci', strlen('oci'))){
    if (!strpos(strtolower(getenv('PDOTEST_DSN')), 'charset=we8mswin1252')) die('skip expected output valid for Oracle with WE8MSWIN1252 character set');
} elseif (!strncasecmp(getenv('PDOTEST_DSN'), 'dblib', strlen('dblib'))) {
    die('skip not for pdo_dblib');
}

?>
--FILE--
<?php

if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

@$db->exec("SET NAMES 'LATIN1'"); // needed for PostgreSQL
$db->exec("CREATE TABLE test (id INTEGER)");
$db->exec("INSERT INTO test (id) VALUES (1)");

$stmt = $db->prepare("SELECT 'Ã' as test FROM test WHERE id = :id");
$stmt->execute(array(":id" => 1));

$row = $stmt->fetch(PDO::FETCH_NUM);
var_dump( $row );

?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "Ã"
}
