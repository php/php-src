--TEST--
PDO_OCI: Attribute: beginTransaction and native transactions
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');
$dbh = PDOTest::factory();

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$dbh->exec("drop table pdo_ac_tab");
$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$dbh->exec("create table pdo_ac_tab (col1 varchar2(25))");

echo "Test 1 Check beginTransaction insertion\n";

$dbh->beginTransaction();
try {
    $dbh->exec("insert into pdo_ac_tab (col1) values ('data 1')");
    $dbh->exec("insert into pdo_ac_tab (col1) values ('data 2')");
    $dbh->commit();
}
catch (PDOException $e) {
    echo "Caught unexpected exception at line " . __LINE__ . "\n";
    echo $e->getMessage() . "\n";
    $dbh->rollback();
}

echo "Test 2 Cause an exception and test beginTransaction rollback\n";

$dbh->beginTransaction();
try {
    $dbh->exec("insert into pdo_ac_tab (col1) values ('not committed #1')");
    $dbh->exec("insert into pdo_ac_tab (col1) values ('data that is too long to fit and will barf')");
    $dbh->commit();
}
catch (PDOException $e) {
    echo "Caught expected exception at line " . __LINE__ . "\n";
    echo $e->getMessage() . "\n";
    $dbh->rollback();
}

echo "Test 3 Setting ATTR_AUTOCOMMIT to true will commit and end the transaction\n";

$dbh->exec("insert into pdo_ac_tab (col1) values ('data 3')");
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);
print "PDO::ATTR_AUTOCOMMIT: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
try {
    $dbh->rollback();
}
catch (PDOException $e) {
    echo "Caught expected exception at line " . __LINE__ . "\n";
    echo $e->getMessage() . "\n";
}

echo "Test 4 Setting ATTR_AUTOCOMMIT to false will commit and end the transaction\n";

$dbh->beginTransaction();
$dbh->exec("insert into pdo_ac_tab (col1) values ('data 4')");
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);
print "PDO::ATTR_AUTOCOMMIT: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));
try {
    $dbh->rollback();
}
catch (PDOException $e) {
    echo "Caught expected exception at line " . __LINE__ . "\n";
    echo $e->getMessage() . "\n";
}

echo "Test 5 Handle transactions ourselves\n";

print "PDO::ATTR_AUTOCOMMIT: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));

$dbh->exec("insert into pdo_ac_tab (col1) values ('not committed #2')");
$dbh->exec("rollback");
$dbh->exec("insert into pdo_ac_tab (col1) values ('data 5')");
$dbh->exec("insert into pdo_ac_tab (col1) values ('data 6')");

$dbh->exec("commit");

// Open new connection to really verify what was inserted

$dbh2 = PDOTest::factory();

echo "Query Results are:\n";
$s = $dbh2->prepare("select col1 from pdo_ac_tab");
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Done\n";

?>
--EXPECTF--
Test 1 Check beginTransaction insertion
Test 2 Cause an exception and test beginTransaction rollback
Caught expected exception at line 35
SQLSTATE[HY000]: General error: 12899 OCIStmtExecute: ORA-12899: %s
%s
Test 3 Setting ATTR_AUTOCOMMIT to true will commit and end the transaction
PDO::ATTR_AUTOCOMMIT: bool(true)
Caught expected exception at line %d
There is no active transaction
Test 4 Setting ATTR_AUTOCOMMIT to false will commit and end the transaction
PDO::ATTR_AUTOCOMMIT: bool(false)
Caught expected exception at line %d
There is no active transaction
Test 5 Handle transactions ourselves
PDO::ATTR_AUTOCOMMIT: bool(false)
Query Results are:
data 1
data 2
data 3
data 4
data 5
data 6
Done
