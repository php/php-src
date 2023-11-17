--TEST--
PDO_Firebird: transaction isolation level
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");
unset($dbh);

$levelStrs = [
    'PDO::FB_READ_COMMITTED',
    'PDO::FB_REPEATABLE_READ',
    'PDO::FB_SERIALIZABLE',
];

echo "Set attr in construct\n";

foreach ($levelStrs as $levelStr) {
    $level = constant($levelStr);
    $dbh = new PDO(
        PDO_FIREBIRD_TEST_DSN,
        PDO_FIREBIRD_TEST_USER,
        PDO_FIREBIRD_TEST_PASS,
        [
            PDO::FB_TRANSACTION_ISOLATION_LEVEL => $level,
        ],
    );

    if ($dbh->getAttribute(PDO::FB_TRANSACTION_ISOLATION_LEVEL) === $level) {
        echo "OK: {$levelStr}\n";
    } else {
        echo "NG: {$levelStr}\n";
    }

    unset($dbh);
}

echo "\n";
echo "Set attr in setAttribute\n";

$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
);

foreach ($levelStrs as $levelStr) {
    $level = constant($levelStr);

    var_dump($dbh->setAttribute(PDO::FB_TRANSACTION_ISOLATION_LEVEL, $level));

    if ($dbh->getAttribute(PDO::FB_TRANSACTION_ISOLATION_LEVEL) === $level) {
        echo "OK: {$levelStr}\n";
    } else {
        echo "NG: {$levelStr}\n";
    }
}

unset($dbh);
?>
--EXPECT--
Set attr in construct
OK: PDO::FB_READ_COMMITTED
OK: PDO::FB_REPEATABLE_READ
OK: PDO::FB_SERIALIZABLE

Set attr in setAttribute
bool(true)
OK: PDO::FB_READ_COMMITTED
bool(true)
OK: PDO::FB_REPEATABLE_READ
bool(true)
OK: PDO::FB_SERIALIZABLE
