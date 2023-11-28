--TEST--
PDO_Firebird: transaction isolation level (Testing for behavior)
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

$table = 'txn_isolation_level_behavior';

$dbh_other = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
);
$dbh_other->query("CREATE TABLE {$table} (val INT)");


echo "========== default(REPEATABLE READ) ==========\n";
$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
);
echo "begin transaction\n";
$dbh->beginTransaction();

echo "insert by other transaction\n";
$dbh_other->exec("INSERT INTO {$table} VALUES (20)");
echo "Read\n";
$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "Close transaction and reset table\n";
$dbh->commit();
$dbh_other->exec("DELETE FROM {$table}");
unset($dbh);
echo "\n";


echo "========== READ COMMITTED ==========\n";
$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
    [PDO::FB_TRANSACTION_ISOLATION_LEVEL => PDO::FB_READ_COMMITTED]
);
echo "begin transaction\n";
$dbh->beginTransaction();

echo "insert by other transaction\n";
$dbh_other->exec("INSERT INTO {$table} VALUES (20)");
echo "Read\n";
$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "Close transaction and reset table\n";
$dbh->commit();
$dbh_other->exec("DELETE FROM {$table}");
unset($dbh);
echo "\n";


echo "========== REPEATABLE READ ==========\n";
$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
    [PDO::FB_TRANSACTION_ISOLATION_LEVEL => PDO::FB_REPEATABLE_READ]
);
echo "begin transaction\n";
$dbh->beginTransaction();

echo "insert by other transaction\n";
$dbh_other->exec("INSERT INTO {$table} VALUES (20)");
echo "Read\n";
$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "Close transaction and reset table\n";
$dbh->commit();
$dbh_other->exec("DELETE FROM {$table}");
unset($dbh);
echo "\n";


/*
 * SERIALIZABLE imposes a strong lock, so the lock will not be released and
 * the test will never end. There is currently no way to confirm.
 * If we can set the lock timeout, it might be possible to test it, so I'll leave it as is.
 */

/*
echo "========== SERIALIZABLE ==========\n";
$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
    [PDO::FB_TRANSACTION_ISOLATION_LEVEL => PDO::FB_SERIALIZABLE]
);
echo "begin transaction\n";
$dbh->beginTransaction();

echo "insert by other transaction\n";
$dbh_other->exec("INSERT INTO {$table} VALUES (20)");
echo "Read\n";
$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "Close transaction and reset table\n";
$dbh->commit();
$dbh_other->exec("DELETE FROM {$table}");
echo "\n";
*/

unset($dbh);

echo "done!";
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec('DROP TABLE txn_isolation_level_behavior');
unset($dbh);
?>
--EXPECT--
========== default(REPEATABLE READ) ==========
begin transaction
insert by other transaction
Read
array(0) {
}
Close transaction and reset table

========== READ COMMITTED ==========
begin transaction
insert by other transaction
Read
array(1) {
  [0]=>
  array(2) {
    ["VAL"]=>
    int(20)
    [0]=>
    int(20)
  }
}
Close transaction and reset table

========== REPEATABLE READ ==========
begin transaction
insert by other transaction
Read
array(0) {
}
Close transaction and reset table

done!
