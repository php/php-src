--TEST--
PDO Common: Bug #72368 (PdoStatement->execute() fails but does not throw an exception)
--EXTENSIONS--
pdo
--SKIPIF--
<?php # vim:ft=php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.dirname(__FILE__) . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$params = [":bar" => 1];
$sql = "SELECT 1";

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, false);
try {
	$stmt = $db->prepare($sql);
	var_dump($stmt->execute($params));
} catch (PDOException $e) {
	var_dump('ERR');
}


$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
try {
	$stmt = $db->prepare($sql);
	var_dump($stmt->execute($params));
} catch (PDOException $e) {
	var_dump('ERR');
}

?>
===DONE===
--EXPECT--
string(3) "ERR"
string(3) "ERR"
===DONE===
