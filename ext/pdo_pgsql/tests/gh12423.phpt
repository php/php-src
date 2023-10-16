--TEST--
GitHub #12424 (Fix GH-12423: [pdo_pgsql] Changed to prioritize DSN authentication information over arguments.)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
require __DIR__ . '/config.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$dsnWithCredentials = $config['ENV']['PDOTEST_DSN'];
$user = $config['ENV']['PDOTEST_USER'] ?? null;
$password = $config['ENV']['PDOTEST_PASS'] ?? null;
if (!$user) {
    preg_match('/user=(.*?) /', $dsnWithCredentials, $match);
    $user = $match[1] ?? '';
}
if (!$password) {
    preg_match('/password=(.*?) /', $dsnWithCredentials, $match);
    $password = $match[1] ?? '';
}
$dsn = str_replace(" user={$user} password={$password}", '', $dsnWithCredentials);

echo "dsn without credentials / correct user / correct password\n";
try {
    $db = new PDO($dsn, $user, $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
} catch (PDOException $e) {
    echo $e->getMessage();
}

echo "dsn with credentials / no user / no password\n";
try {
    $db = new PDO("{$dsn} user={$user} password={$password}", null, null, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
} catch (PDOException $e) {
    echo $e->getMessage();
}

echo "dsn with correct user / incorrect user / correct password\n";
try {
    $db = new PDO("{$dsn} user={$user}", 'hoge', $password, [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
} catch (PDOException $e) {
    echo $e->getMessage();
}

echo "dsn with correct password / correct user / incorrect password\n";
try {
    $db = new PDO("{$dsn} password={$password}", $user, 'fuga', [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n\n";
} catch (PDOException $e) {
    echo $e->getMessage();
}

echo "dsn with correct credentials / incorrect user / incorrect password\n";
try {
    $db = new PDO("{$dsn} user={$user} password={$password}", 'hoge', 'fuga', [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
    echo "Connected.\n";
} catch (PDOException $e) {
    echo $e->getMessage();
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
