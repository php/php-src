--TEST--
PDO MySQL: reusing a persistent connection resets its session state (COM_RESET_CONNECTION)
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
// Reusing a pooled persistent connection sends COM_RESET_CONNECTION, which must
// wipe all existing session state.

require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$dsn  = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;
$opts = [PDO::ATTR_PERSISTENT => true, PDO::ATTR_ERRMODE => PDO::ERRMODE_SILENT];

// Establish a persistent handle with some session state.
$db1  = new PDO($dsn, $user, $pass, $opts);
$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();

$db1->exec('SET @test_var = 42');
$db1->exec('CREATE TEMPORARY TABLE pdo_reset_tmp (id INT)');

$db1->exec('DROP TABLE IF EXISTS pdo_reset_trx');
$db1->exec('CREATE TABLE pdo_reset_trx (id INT) ENGINE=InnoDB');

$db1->beginTransaction();
$db1->exec('INSERT INTO pdo_reset_trx VALUES (1)');

// Return the connection to the pool.
$db1 = null;

// Open a second handle for the same DSN, which should be reset.
$db2  = new PDO($dsn, $user, $pass, $opts);
$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();

// Compare the connection IDs to ensure they are different.
if ($con1 != $con2) {
    printf("[001] Expected the pooled connection to be reused (%s vs %s)\n",
           var_export($con1, true), var_export($con2, true));
}

// Check the session variable to ensure it is no longer present.
$tmp = $db2->query('SELECT @test_var')->fetchColumn();

if ($tmp !== null) {
    printf("[002] User variable should be reset, got %s\n", var_export($tmp, true));
}

// Check the temporary table to ensure it is no longer present.
if ($db2->query('SELECT COUNT(*) FROM pdo_reset_tmp') !== false) {
    printf("[003] Temporary table should not exist after reset\n");
}

// Check to see we are no longer in a transaction.
$rows = $db2->query('SELECT COUNT(*) FROM pdo_reset_trx')->fetchColumn();

if ($rows != 0) {
    printf("[004] Transaction should have been rolled back, found %s row(s)\n", var_export($rows, true));
}

// Clean up the non-temporary table.
$db2->exec('DROP TABLE IF EXISTS pdo_reset_trx');

echo "done!";
?>
--EXPECT--
done!
