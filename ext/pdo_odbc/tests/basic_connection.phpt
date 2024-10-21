--TEST--
Basic test for connection. (When not using a DSN alias)
--EXTENSIONS--
pdo_odbc
--SKIPIF--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
ODBCPDOTest::skipNoDirect();
ODBCPDOTest::skipToofewCredentials();
ODBCPDOTest::skip();
?>
--XLEAK--
A bug in msodbcsql causes a memory leak when reconnecting after closing. See GH-12306
--FILE--
<?php
require_once __DIR__ . '/inc/odbc_pdo_test.inc';
$dsnWithCredentials = PDO_ODBC_TEST_DSN;
$user = PDO_ODBC_TEST_USER;
$password = PDO_ODBC_TEST_PASS;

$dsn = str_replace(";uid={$user};pwd={$password}", '', $dsnWithCredentials);

echo "dsn without credentials / correct user / correct password\n";
try {
    $db = new PDO($dsn, $user, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
    $db = null;
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}

echo "dsn with credentials / no user / no password\n";
try {
    $db = new PDO("{$dsn};uid={$user};pwd={$password}", null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
    $db = null;
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}

echo "dsn with correct user / incorrect user / correct password\n";
try {
    $db = new PDO("{$dsn};UID={$user}", 'hoge', $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
    $db = null;
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}

echo "dsn with correct password / correct user / incorrect password\n";
try {
    $db = new PDO("{$dsn};PWD={$password}", $user, 'fuga', [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
    $db = null;
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}

echo "dsn with correct credentials / incorrect user / incorrect password\n";
try {
    $db = new PDO("{$dsn};Uid={$user};Pwd={$password}", 'hoge', 'fuga', [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n";
    $db = null;
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECT--
dsn without credentials / correct user / correct password
Connected.

dsn with credentials / no user / no password
Connected.

dsn with correct user / incorrect user / correct password
Connected.

dsn with correct password / correct user / incorrect password
Connected.

dsn with correct credentials / incorrect user / incorrect password
Connected.
