--TEST--
PDOStatement::fetchColumn() invalid column index
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

function fetchColumn($stmt, $index) {
    $stmt->execute();
    return $stmt->fetchColumn($index);
}

$conn  = PDOTest::factory();
$query = 'SELECT 1';

switch ($conn->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'oci':
        $query .= ' FROM DUAL';
        break;
    case 'firebird':
        $query .= ' FROM RDB$DATABASE';
        break;
}

$stmt = $conn->prepare($query);

var_dump(fetchColumn($stmt, -1));
var_dump(fetchColumn($stmt, 0));
var_dump(fetchColumn($stmt, 1));
?>
--EXPECTF--
Warning: PDOStatement::fetchColumn(): SQLSTATE[HY000]: General error: Invalid column index in %s
bool(false)
string(1) "1"

Warning: PDOStatement::fetchColumn(): SQLSTATE[HY000]: General error: Invalid column index in %s
bool(false)
