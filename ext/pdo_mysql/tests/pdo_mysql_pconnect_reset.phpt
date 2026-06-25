--TEST--
PDO MySQL persistent connection session state is reset (COM_RESET_CONNECTION) on reuse
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$dsn  = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;

$db1 = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true]);
$db1->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();

/* leave some server-side session state behind */
$db1->exec('SET @test_var = 42');
$db1->exec('CREATE TEMPORARY TABLE pdo_reset_tmp (id INT)');

$tmp = $db1->query('SELECT @test_var')->fetchColumn();
if ($tmp != 42)
    printf("[001] Expected 42, got %s\n", var_export($tmp, true));

/* release the handle; the persistent connection stays pooled */
$db1 = null;

/* reusing the pooled connection must reset its session state */
$db2 = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true]);
$db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();
if ($con1 != $con2)
    printf("[002] Expected the pooled connection to be reused (%s vs %s)\n",
        var_export($con1, true), var_export($con2, true));

$tmp = $db2->query('SELECT @test_var')->fetchColumn();
if (null !== $tmp)
    printf("[003] User variable should have been reset, got %s\n", var_export($tmp, true));

$stmt = $db2->query("SHOW TABLES LIKE 'pdo_reset_tmp'");
if (false !== $stmt->fetch())
    printf("[004] Temporary table should not exist after reset\n");

echo "done!";
?>
--EXPECT--
done!
