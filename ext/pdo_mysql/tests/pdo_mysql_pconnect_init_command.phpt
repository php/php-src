--TEST--
PDO MySQL persistent connection re-runs INIT_COMMAND after COM_RESET_CONNECTION reuse
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
$opts = [
    PDO::ATTR_PERSISTENT          => true,
    Pdo\Mysql::ATTR_INIT_COMMAND  => 'SET @init_marker = 42',
];

$db1 = new PDO($dsn, $user, $pass, $opts);
$db1->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();
if ($db1->query('SELECT @init_marker')->fetchColumn() != 42)
    printf("[001] INIT_COMMAND should have set @init_marker\n");

$db1 = null; /* return to the pool */

/* reuse fires COM_RESET_CONNECTION, which clears @init_marker; INIT_COMMAND must be re-run */
$db2 = new PDO($dsn, $user, $pass, $opts);
$db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();
if ($con1 != $con2)
    printf("[002] Expected the pooled connection to be reused\n");

if ($db2->query('SELECT @init_marker')->fetchColumn() != 42)
    printf("[003] INIT_COMMAND should be re-run after reuse (got %s)\n",
        var_export($db2->query('SELECT @init_marker')->fetchColumn(), true));

echo "done!";
?>
--EXPECT--
done!
