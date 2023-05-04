--TEST--
PDO Common: errorCode()
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
if (str_starts_with(getenv('PDOTEST_DSN'), "firebird")) die('xfail firebird driver does not have empty initial errorCode');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');

$dsn = getenv('PDOTEST_DSN');
$user = getenv('PDOTEST_USER');
$pass = getenv('PDOTEST_PASS');
$attr = getenv('PDOTEST_ATTR');
if (is_string($attr) && strlen($attr)) {
    $attr = unserialize($attr);
} else {
    $attr = null;
}

if ($user === false) $user = NULL;
if ($pass === false) $pass = NULL;

$conn = new PDO($dsn, $user, $pass, $attr);

var_dump($conn->errorCode());

$query = 'SELECT 1';
if ($conn->getAttribute(PDO::ATTR_DRIVER_NAME) === 'oci') {
    $query .= ' FROM DUAL';
}
var_dump($conn->errorCode());
var_dump($conn->errorCode());

$stmt = $conn->prepare($query);
var_dump($conn->errorCode());
var_dump($stmt->errorCode());

$stmt->execute();
var_dump($stmt->errorCode());
var_dump($stmt->errorCode());

?>
--EXPECT--
NULL
string(5) "00000"
string(5) "00000"
string(5) "00000"
NULL
string(5) "00000"
string(5) "00000"
