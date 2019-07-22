--TEST--
PDO Common: FR #71885 (Allow escaping question mark placeholders)
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'pgsql', strlen('pgsql'))) die('skip not relevant for pgsql driver');
if (!strncasecmp(getenv('PDOTEST_DSN'), 'odbc', strlen('odbc'))) die('skip inconsistent error message with odbc');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$db->exec("CREATE TABLE test (a int)");

$sql = "SELECT * FROM test WHERE a ?? 1";

try {
	$db->exec($sql);
} catch (PDOException $e) {
	var_dump(strpos($e->getMessage(), "?") !== false);
}

try {
	$stmt = $db->prepare($sql);
	$stmt->execute();
} catch (PDOException $e) {
	var_dump(strpos($e->getMessage(), "?") !== false);
}

if ($db->getAttribute(PDO::ATTR_DRIVER_NAME) == 'mysql') {
	$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
}

try {
	$stmt = $db->prepare($sql);
	$stmt->execute();
} catch (PDOException $e) {
	var_dump(strpos($e->getMessage(), "?") !== false);
}

?>
===DONE===
--EXPECT--
bool(true)
bool(true)
bool(true)
===DONE===
