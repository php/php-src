--TEST--
PDO_Firebird: transaction isolation level (Testing for setting attribute values)
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require("testdb.inc");
$dbh = getDbConnection();
unset($dbh);

$levelStrs = [
    'Pdo\\Firebird::READ_COMMITTED',
    'Pdo\\Firebird::REPEATABLE_READ',
    'Pdo\\Firebird::SERIALIZABLE',
];

echo "========== Set attr in construct ==========\n";

foreach ($levelStrs as $levelStr) {
    $level = constant($levelStr);
    $dbh = new PDO(
        PDO_FIREBIRD_TEST_DSN,
        PDO_FIREBIRD_TEST_USER,
        PDO_FIREBIRD_TEST_PASS,
        [
            Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL => $level,
        ],
    );

    if ($dbh->getAttribute(Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL) === $level) {
        echo "OK: {$levelStr}\n";
    } else {
        echo "NG: {$levelStr}\n";
    }

    unset($dbh);
}

echo "Invalid value\n";
try {
    $dbh = new PDO(
        PDO_FIREBIRD_TEST_DSN,
        PDO_FIREBIRD_TEST_USER,
        PDO_FIREBIRD_TEST_PASS,
        [
            Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL => PDO::ATTR_AUTOCOMMIT, // Invalid value
        ],
    );
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}

unset($dbh);

echo "\n";
echo "========== Set attr in setAttribute ==========\n";

$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
);

foreach ($levelStrs as $levelStr) {
    $level = constant($levelStr);

    var_dump($dbh->setAttribute(Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL, $level));

    if ($dbh->getAttribute(Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL) === $level) {
        echo "OK: {$levelStr}\n";
    } else {
        echo "NG: {$levelStr}\n";
    }
}

echo "Invalid value\n";
try {
    $dbh->setAttribute(Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL, PDO::ATTR_AUTOCOMMIT); // Invalid value
} catch (Throwable $e) {
    echo $e->getMessage()."\n";
}

unset($dbh);
?>
--EXPECT--
========== Set attr in construct ==========
OK: Pdo\Firebird::READ_COMMITTED
OK: Pdo\Firebird::REPEATABLE_READ
OK: Pdo\Firebird::SERIALIZABLE
Invalid value
Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL must be a valid transaction isolation level (Pdo\Firebird::READ_COMMITTED, Pdo\Firebird::REPEATABLE_READ, or Pdo\Firebird::SERIALIZABLE)

========== Set attr in setAttribute ==========
bool(true)
OK: Pdo\Firebird::READ_COMMITTED
bool(true)
OK: Pdo\Firebird::REPEATABLE_READ
bool(true)
OK: Pdo\Firebird::SERIALIZABLE
Invalid value
Pdo\Firebird::TRANSACTION_ISOLATION_LEVEL must be a valid transaction isolation level (Pdo\Firebird::READ_COMMITTED, Pdo\Firebird::REPEATABLE_READ, or Pdo\Firebird::SERIALIZABLE)
