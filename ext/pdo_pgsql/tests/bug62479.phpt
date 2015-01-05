--TEST--
PDO PgSQL Bug #62479 (PDO-psql cannot connect if password contains spaces)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();

$dsn = getenv('PDOTEST_DSN');
if (empty($dsn)) die('skip no dsn found in env');

$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
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
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$pdo = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$pdo->setAttribute(PDO::ATTR_EMULATE_PREPARES, true);
$rand = rand(5, 400);
$user = "pdo_test_$rand";
$template = "CREATE USER $user WITH PASSWORD '%s'";
$dropUser = "DROP USER $user";
$testQuery = 'SELECT 1 as verification';

// Create temp user with space in password
$sql = sprintf($template, 'my password');
$pdo->query($sql);
$testConn = new PDO($conf['ENV']['PDOTEST_DSN'], $user, "my password");
$result = $testConn->query($testQuery)->fetch();
$check = $result[0];
var_dump($check);

// Remove the user
$pdo->query($dropUser);

// Create a user with a space and single quote
$sql = sprintf($template, "my pass''word");
$pdo->query($sql);

$testConn = new PDO($conf['ENV']['PDOTEST_DSN'], $user, "my pass'word");
$result = $testConn->query($testQuery)->fetch();
$check = $result[0];
var_dump($check);

// Remove the user
$pdo->query($dropUser);
?>
--EXPECT--
int(1)
int(1)

