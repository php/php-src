--TEST--
PDO MySQL: reusing a persistent connection re-applies its connection config after reset
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
// COM_RESET_CONNECTION reverts the session to server defaults; mysqlnd should
// then re-apply the connection config, so a reused connection stays configured.

require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$dsn  = MySQLPDOTest::getDSN() . ';charset=latin1';
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;
$opts = [
    PDO::ATTR_PERSISTENT         => true,
    PDO::ATTR_ERRMODE            => PDO::ERRMODE_SILENT,
    Pdo\Mysql::ATTR_INIT_COMMAND => 'SET @init_marker = 42',
];


// Establish a persistent handle with the above options.
$db1  = new PDO($dsn, $user, $pass, $opts);
$con1 = $db1->query('SELECT CONNECTION_ID()')->fetchColumn();

// Sanity check that the charset was configured.
$charset = $db1->query('SELECT @@session.character_set_connection')->fetchColumn();

if ($charset !== 'latin1') {
    printf("[001] Charset should be latin1 at connect, got %s\n", var_export($charset, true));
}

// Sanity check that the ATTR_INIT_COMMAND was executed.
$marker = $db1->query('SELECT @init_marker')->fetchColumn();

if ($marker != 42) {
    printf("[002] Init command should have set @init_marker at connect, got %s\n", var_export($marker, true));
}

// Return the connection to the pool.
$db1 = null;

// Open a second handle for the same DSN, which should be reset, but preserve
// the options from the initial configuration.
$db2  = new PDO($dsn, $user, $pass, $opts);
$con2 = $db2->query('SELECT CONNECTION_ID()')->fetchColumn();

// Compare the connection IDs to ensure they are the same.
if ($con1 != $con2) {
    printf("[003] Expected the pooled connection to be reused (%s vs %s)\n",
        var_export($con1, true), var_export($con2, true));
}

// Check the charset to ensure we're using the one configured in the DSN.
$charset = $db2->query('SELECT @@session.character_set_connection')->fetchColumn();

if ($charset !== 'latin1') {
    printf("[004] Charset should be re-applied to latin1, got %s\n", var_export($charset, true));
}

// Check the session variable to ensure the ATTR_INIT_COMMAND was re-executed.
$marker = $db2->query('SELECT @init_marker')->fetchColumn();

if ($marker != 42) {
    printf("[005] Init command should be re-run after reuse, got %s\n", var_export($marker, true));
}

echo "done!";
?>
--EXPECT--
done!
