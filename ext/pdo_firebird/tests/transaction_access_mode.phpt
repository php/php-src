--TEST--
PDO_Firebird: transaction access mode
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

$table = 'transaction_access_mode';

$values = [
    ['val' => true, 'label' => 'writable'],
    ['val' => false, 'label' => 'readonly'],
];

echo "Set attr in construct\n";

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
echo "Set attr in setAttribute and behavior check\n";

$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
);

$dbh->query("CREATE TABLE {$table} (val int)");

var_dump($dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, true));

if ($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION) === true) {
    echo "OK: writable\n";
} else {
    echo "NG: writable\n";
}
$dbh->query("INSERT INTO {$table} VALUES (12)");
$r = $dbh->query("SELECT * FROM {$table}");
foreach ($r as $row) {
    var_dump($row);
}

var_dump($dbh->setAttribute(PDO::FB_WRITABLE_TRANSACTION, false));

if ($dbh->getAttribute(PDO::FB_WRITABLE_TRANSACTION) === false) {
    echo "OK: readonly\n";
} else {
    echo "NG: readonly\n";
}
try {
    $dbh->query("INSERT INTO {$table} VALUES (19)");
} catch (PDOException $e) {
    echo "error with readonly\n";
}
$r = $dbh->query("SELECT * FROM {$table}");
foreach ($r as $row) {
    var_dump($row);
}

unset($dbh);
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec('DROP TABLE transaction_access_mode');
unset($dbh);
?>
--EXPECT--
Set attr in construct
OK: writable
OK: readonly

Set attr in setAttribute and behavior check
bool(true)
OK: writable
array(2) {
  ["VAL"]=>
  int(12)
  [0]=>
  int(12)
}
bool(true)
OK: readonly
error with readonly
array(2) {
  ["VAL"]=>
  int(12)
  [0]=>
  int(12)
}
