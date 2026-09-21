--TEST--
Persistent reconnect after a dead cached handle does not leak the old pdo_dbh_t
--EXTENSIONS--
pdo_mysql
--ENV--
VALGRIND_OPTS=--leak-check=full
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
if (!MySQLPDOTest::isPDOMySQLnd()) {
    die('skip mysqlnd only (libmysql mysql_ping auto-reconnects)');
}
?>
--FILE--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';

$dsn = MySQLPDOTest::getDSN();
$user = PDO_MYSQL_TEST_USER;
$pass = PDO_MYSQL_TEST_PASS;
$opts = [
    PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
    PDO::ATTR_PERSISTENT => true,
];

$cached = new PDO($dsn, $user, $pass, $opts);
$id = (int) $cached->query('SELECT CONNECTION_ID()')->fetchColumn();
unset($cached);

$killer = new PDO($dsn, $user, $pass, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
$killer->exec('KILL ' . $id);
unset($killer);

$next = new PDO($dsn, $user, $pass, $opts);
$nextId = (int) $next->query('SELECT CONNECTION_ID()')->fetchColumn();
var_dump($nextId !== $id);
echo $next->query('SELECT 1')->fetchColumn(), "\n";
?>
--EXPECT--
bool(true)
1
