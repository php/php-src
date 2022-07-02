--TEST--
PDOStatement::fetchColumn() invalid column index
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

try {
    var_dump(fetchColumn($stmt, -1));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
var_dump(fetchColumn($stmt, 0));
try {
    var_dump(fetchColumn($stmt, 1));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
Column index must be greater than or equal to 0
string(1) "1"
Invalid column index
