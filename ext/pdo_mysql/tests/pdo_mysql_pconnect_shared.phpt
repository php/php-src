--TEST--
PDO MySQL: a persistent connection shared by two live handles is not reset
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
// The reset should only fire when an idle pooled connection is handed back out,
// never while another handle is still using it. Two persistent handles for the
// same DSN share one connection, so opening the second must not reset the
// session.

require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$dsn  = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;
$opts = [PDO::ATTR_PERSISTENT => true, PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT];

// Establish a persistent handle with some session state.
$db1 = new PDO($dsn, $user, $pass, $opts);
$db1->exec('SET @test_var = 42');
$db1->exec('CREATE TEMPORARY TABLE pdo_shared_tmp (id INT)');

// Open a second handle for the same DSN while $db1 is still alive.
$db2 = new PDO($dsn, $user, $pass, $opts);

// Compare the connection IDs to ensure they are the same.
$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();
$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();

if ($con1 != $con2) {
    printf("[001] Expected both handles to share one connection (%s vs %s)\n",
           var_export($con1, true), var_export($con2, true));
}

// Check the session variable to ensure it is the same.
$tmp = $db1->query('SELECT @test_var')->fetchColumn();

if ($tmp != 42) {
    printf("[002] User variable should be preserved, got %s\n", var_export($tmp, true));
}

// Ensure the temporary table still exists.
if (false === $db1->query('SELECT COUNT(*) FROM pdo_shared_tmp')) {
    printf("[003] Temporary table should still exist\n");
}

echo "done!";
?>
--EXPECT--
done!
