--TEST--
Bug #80892 PDO::PARAM_INT is treated the same as PDO::PARAM_STR
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';

/** @var PDO $db */
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_DEFAULT_FETCH_MODE, PDO::FETCH_ASSOC);

$db->exec("CREATE OR REPLACE FUNCTION bug80892 (x bigint) RETURNS varchar AS $$ SELECT 'bigint' $$ LANGUAGE sql");
$db->exec("CREATE OR REPLACE FUNCTION bug80892 (x int) RETURNS varchar AS $$ SELECT 'int' $$ LANGUAGE sql");
$db->exec("CREATE OR REPLACE FUNCTION bug80892 (x text) RETURNS varchar AS $$ SELECT 'text' $$ LANGUAGE sql");

// Sanity check
var_dump($db->query("SELECT bug80892(2147483648)")->fetchColumn());
var_dump($db->query("SELECT bug80892(1)")->fetchColumn());
var_dump($db->query("SELECT bug80892('1')")->fetchColumn());

// No binding
$stmt = $db->prepare("SELECT bug80892(?)");
$stmt->execute([1]);
var_dump($stmt->fetchColumn());

// Bind int value as string
$stmt = $db->prepare("SELECT bug80892(?)");
$stmt->bindValue(1, 1, PDO::PARAM_STR);
$stmt->execute();
var_dump($stmt->fetchColumn());

// Bind int value as int
$stmt = $db->prepare("SELECT bug80892(?)");
$stmt->bindValue(1, 1, PDO::PARAM_INT);
$stmt->execute();
var_dump($stmt->fetchColumn());

// Bind string value as int
$stmt = $db->prepare("SELECT bug80892(?)");
$stmt->bindValue(1, '1', PDO::PARAM_INT);
$stmt->execute();
var_dump($stmt->fetchColumn());

// Bind bigint string value as int
$stmt = $db->prepare("SELECT bug80892(?)");
$stmt->bindValue(1, '2147483648', PDO::PARAM_INT);
$stmt->execute();
var_dump($stmt->fetchColumn());

// Bind negative bigint (string on 32bit) as int
$stmt = $db->prepare("SELECT bug80892(?)");
$stmt->bindValue(1, PHP_INT_SIZE == 4 ? '-33333333333' : -33333333333, PDO::PARAM_INT);
$stmt->execute();
var_dump($stmt->fetchColumn());

$db->exec("DROP FUNCTION bug80892 (bigint)");
$db->exec("DROP FUNCTION bug80892 (int)");
$db->exec("DROP FUNCTION bug80892 (text)");

?>
--EXPECT--
string(6) "bigint"
string(3) "int"
string(4) "text"
string(4) "text"
string(4) "text"
string(3) "int"
string(3) "int"
string(6) "bigint"
string(6) "bigint"
