--TEST--
PDO Common: Bug #34687 (query doesn't return error information)
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
require getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$x = $db->query("UPDATE non_existent_pdo_test_table set foo = 'bar'");

var_dump($x);
$code = $db->errorCode();
if ($code !== '00000' && strlen($code)) {
    echo "OK: $code\n";
} else {
    echo "ERR: $code\n";
    print_r($db->errorInfo());
}

?>
--EXPECTF--
bool(false)
OK: %s
