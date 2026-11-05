--TEST--
PDO_DBLIB: Set query timeouts
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
require __DIR__ . '/config.inc';
$dsn = getenv('PDO_ODBC_TEST_DSN');
if (!$dsn || strpos($dsn, '=') === false) {
    die('skip');
}
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$dsn = getenv('PDO_ODBC_TEST_DSN');
$user = getenv('PDO_ODBC_TEST_USER');
$password = getenv('PDO_ODBC_TEST_PASS');

$sql = 'WAITFOR DELAY \'00:00:02\'';

// regular timeout attribute, set after instance created, will affect query timeout, causing this query to fail
$db = new PDO($dsn, $user, $password);
$db->setAttribute(PDO::ATTR_TIMEOUT, 1);
$stmt = $db->prepare($sql);
try {
    $stmt->execute();
    echo "Shouldn't happen\n";
} catch (PDOException) {
    echo "OK for timeout set after\n";
}

// regular timeout attribute will affect query timeout, causing this query to fail
$attributes = [PDO::ATTR_TIMEOUT => 1];
$db = new PDO($dsn, $user, $password, $attributes);
$stmt = $db->prepare($sql);
try {
    $stmt->execute();
    echo "Shouldn't happen\n";
} catch (PDOException) {
    echo "OK for timeout set on ctor\n";
}

?>
--EXPECT--
OK for timeout set after
OK for timeout set on ctor
