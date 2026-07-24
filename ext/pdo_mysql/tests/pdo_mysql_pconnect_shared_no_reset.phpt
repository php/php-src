--TEST--
PDO MySQL persistent connection shared by two handles is not reset on reuse
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

/* server-side session state on the shared connection */
$db1->exec('SET @test_var = 42');
$db1->exec('CREATE TEMPORARY TABLE pdo_shared_tmp (id INT)');

/* Opening a second handle for the same DSN joins the same underlying
 * connection while $db1 is still using it. That must not reset the session. */
$db2 = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true]);
$db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

/* same physical connection */
$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();
$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();
if ($con1 != $con2)
    printf("[001] Expected both handles to share one connection (%s vs %s)\n",
        var_export($con1, true), var_export($con2, true));

/* state set before $db2 existed must survive */
$tmp = $db1->query('SELECT @test_var')->fetchColumn();
if ($tmp != 42)
    printf("[002] User variable should be preserved, got %s\n", var_export($tmp, true));

/* SHOW TABLES never lists temporary tables, so probe by selecting from it */
if (false === $db1->query('SELECT COUNT(*) FROM pdo_shared_tmp'))
    printf("[003] Temporary table should still exist\n");

echo "done!";
?>
--EXPECT--
done!
