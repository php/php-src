--TEST--
PDO MySQL persistent connection restores the DSN charset after COM_RESET_CONNECTION reuse
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

$dsn  = MySQLPDOTest::getDSN() . ';charset=latin1';
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;

$db1 = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true]);
$db1->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();
$cs1  = $db1->query('SELECT @@session.character_set_connection')->fetchColumn();
if ($cs1 !== 'latin1')
    printf("[001] Expected latin1, got %s\n", var_export($cs1, true));

$db1 = null; /* return to the pool */

/* reuse fires COM_RESET_CONNECTION, which reverts the charset; it must be re-applied */
$db2 = new PDO($dsn, $user, $pass, [PDO::ATTR_PERSISTENT => true]);
$db2->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();
if ($con1 != $con2)
    printf("[002] Expected the pooled connection to be reused (%s vs %s)\n",
        var_export($con1, true), var_export($con2, true));

$cs2 = $db2->query('SELECT @@session.character_set_connection')->fetchColumn();
if ($cs2 !== 'latin1')
    printf("[003] Charset should be restored to latin1 after reuse, got %s\n", var_export($cs2, true));

echo "done!";
?>
--EXPECT--
done!
