--TEST--
PDOStatement::checkLiveness()
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

if (!strncasecmp(getenv('PDOTEST_DSN'), 'oci', strlen('oci'))) die('skip cannot set wait_timeout equivalent');

$conn = PDOTest::factory();

try {
    $conn->checkLiveness();
} catch (PDOException $e) {
    die("skip driver doesn't support checkLiveness: " . $e->getMessage());
}

?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$conn = PDOTest::factory();

switch ($conn->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'mysql':
        $conn->exec("SET SESSION wait_timeout = 1");
        break;
    case 'pgsql':
        $conn->exec("SET SESSION idle_in_transaction_session_timeout = 1000");
        break;
}

var_dump($conn->checkLiveness());
sleep(2);
var_dump($conn->checkLiveness());

?>
--EXPECTF--
bool(true)
bool(false)
