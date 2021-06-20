--TEST--
PDO_OCI: Attribute: Basic autocommit functionality
--EXTENSIONS--
pdo
pdo_oci
--SKIPIF--
<?php
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

print "PDO::ATTR_AUTOCOMMIT: Default: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));

echo "Change setting to false - ";

$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);

print "PDO::ATTR_AUTOCOMMIT: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));

echo "Change setting back to true - ";

$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);

print "PDO::ATTR_AUTOCOMMIT: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));

// Use 2nd connection to check that autocommit does commit

echo "Insert data\n";
$dbh->exec("create table pdo_ac_tab (col1 varchar2(20))");
$dbh->exec("insert into pdo_ac_tab (col1) values ('some data')");

$dbh2 = PDOTest::factory();

echo "Second connection should be able to see committed data\n";
$s = $dbh2->prepare("select col1 from pdo_ac_tab");
$s->execute();
while ($r = $s->fetch()) {
    echo "Data is: " . $r[0] . "\n";
}

$dbh->exec("drop table pdo_ac_tab");

echo "Done\n";

?>
--EXPECT--
PDO::ATTR_AUTOCOMMIT: Default: bool(true)
Change setting to false - PDO::ATTR_AUTOCOMMIT: bool(false)
Change setting back to true - PDO::ATTR_AUTOCOMMIT: bool(true)
Insert data
Second connection should be able to see committed data
Data is: some data
Done
