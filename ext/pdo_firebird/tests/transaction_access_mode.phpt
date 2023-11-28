--TEST--
PDO_Firebird: transaction access mode
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
unset($dbh);

$table = 'transaction_access_mode';

$values = [
    ['val' => true, 'label' => 'writable'],
    ['val' => false, 'label' => 'readonly'],
];

echo "========== Set attr in construct ==========\n";

foreach ($values as $value) {
    $dbh = new PDO(
        PDO_FIREBIRD_TEST_DSN,
        PDO_FIREBIRD_TEST_USER,
        PDO_FIREBIRD_TEST_PASS,
        [
            PDO::FB_WRITABLE_TRANSACTION => $value['val'],
        ],
    );

    if ($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION) === $value['val']) {
        echo "OK: {$value['label']}\n";
    } else {
        echo "NG: {$value['label']}\n";
    }

    unset($dbh);
}

echo "\n";
echo "========== Set attr in setAttribute and behavior check ==========\n";

$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
);

$dbh->query("CREATE TABLE {$table} (val INT)");

echo "writable\n";
var_dump($dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, true));
if ($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION) === true) {
    echo "OK: writable\n";
} else {
    echo "NG: writable\n";
}
$dbh->query("INSERT INTO {$table} VALUES (12)");
$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "\n";

echo "readonly\n";
var_dump($dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, false));
if ($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION) === false) {
    echo "OK: readonly\n";
} else {
    echo "NG: readonly\n";
}
try {
    $dbh->query("INSERT INTO {$table} VALUES (19)");
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}
$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "\n";
echo "========== Set attr in setAttribute while transaction ==========\n";

$dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, true);
$dbh->beginTransaction();

echo "writable to writable\n";
try {
    $dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, true);
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}
var_dump($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION));
echo "\n";

echo "writable to readonly\n";
try {
    $dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, false);
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}
var_dump($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION));
echo "\n";

$dbh->commit();
$dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, false);
$dbh->beginTransaction();

echo "readonly to writable\n";
try {
    $dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, true);
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}
var_dump($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION));
echo "\n";

echo "readonly to readonly\n";
try {
    $dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, false);
} catch (PDOException $e) {
    echo $e->getMessage()."\n";
}
var_dump($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION));

unset($dbh);
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec('DROP TABLE transaction_access_mode');
unset($dbh);
?>
--EXPECT--
========== Set attr in construct ==========
OK: writable
OK: readonly

========== Set attr in setAttribute and behavior check ==========
writable
bool(true)
OK: writable
array(1) {
  [0]=>
  array(2) {
    ["VAL"]=>
    int(12)
    [0]=>
    int(12)
  }
}

readonly
bool(true)
OK: readonly
SQLSTATE[42000]: Syntax error or access violation: -817 attempted update during read-only transaction
array(1) {
  [0]=>
  array(2) {
    ["VAL"]=>
    int(12)
    [0]=>
    int(12)
  }
}

========== Set attr in setAttribute while transaction ==========
writable to writable
SQLSTATE[HY000]: General error: Cannot change access mode while a transaction is already open
bool(true)

writable to readonly
SQLSTATE[HY000]: General error: Cannot change access mode while a transaction is already open
bool(true)

readonly to writable
SQLSTATE[HY000]: General error: Cannot change access mode while a transaction is already open
bool(false)

readonly to readonly
SQLSTATE[HY000]: General error: Cannot change access mode while a transaction is already open
bool(false)
