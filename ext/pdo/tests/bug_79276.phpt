--TEST--
Parsing string literals on non-MySQL databases
--SKIPIF--
<?php
if (!extension_loaded('pdo')) die('skip');
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();

$conn = PDOTest::factory();
if ($conn->getAttribute(PDO::ATTR_DRIVER_NAME) === 'mysql') {
    die('skip mysql');
}
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) putenv('REDIR_TEST_DIR='.__DIR__ . '/../../pdo/tests/');
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';

$conn  = PDOTest::factory();
$query = <<<'SQL'
SELECT '\'', ?'
SQL;

switch ($conn->getAttribute(PDO::ATTR_DRIVER_NAME)) {
    case 'oci':
        $query .= ' FROM DUAL';
        break;
    case 'firebird':
        $query .= ' FROM RDB$DATABASE';
        break;
}

$stmt = $conn->prepare($query);
$stmt->execute();
echo $stmt->fetchColumn(), \PHP_EOL;

--EXPECT--
\', ?
