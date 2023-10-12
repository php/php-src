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

[
    'ENV' => [
        'PDOTEST_DSN' => $dsnWithCredentials,
        'PDOTEST_USER' => $user,
        'PDOTEST_PASS' => $password,
    ],
] = __DIR__ . '/common.phpt';

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
