--TEST--
PDO PgSQL Bug #69362 (PDO-pgsql fails to connect if password contains a leading single quote)
--EXTENSIONS--
pdo
pdo_pgsql
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();

$dsn = getenv('PDOTEST_DSN');
if (empty($dsn)) die('skip no dsn found in env');

$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);


$user = 'pdo_test_'.rand(5, 400);
$pass = 'testpass';

// Assume that if we can't create or drop a user, this test needs to be skipped
try {
    $db->exec("DROP USER IF EXISTS $user");
    $db->exec("CREATE USER $user WITH PASSWORD '$pass'");
} catch (PDOException $e) {
    die("skip You need CREATEUSER permissions to run the test");
}

// Peer authentication might prevent the test from properly running
try {
    $testConn = new PDO($dsn, $user, $pass);
} catch (PDOException $e) {
    echo "skip ".$e->getMessage();
}

$db->exec("DROP USER $user");

?>
--FILE--
<?php
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(__DIR__ . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$rand = rand(5, 400);
$user = "pdo_test_$rand";
$template = "CREATE USER $user WITH PASSWORD '%s'";
$dropUser = "DROP USER $user";
$testQuery = 'SELECT 1 as verification';

// Create temp user with leading single quote
$sql = sprintf($template, "''mypassword");
$pdo->query($sql);
$testConn = new PDO($config['ENV']['PDOTEST_DSN'], $user, "'mypassword");
$result = $testConn->query($testQuery)->fetch();
$check = $result[0];
var_dump($check);

// Remove the user
$pdo->query($dropUser);

?>
--EXPECT--
int(1)
