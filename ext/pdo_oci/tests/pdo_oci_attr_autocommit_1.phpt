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
$dbh->exec("create table test_pdo_oci_attr_autocommit_1 (col1 varchar2(20))");
$dbh->exec("insert into test_pdo_oci_attr_autocommit_1 (col1) values ('some data')");

$dbh2 = PDOTest::factory();

echo "Second connection should be able to see committed data\n";
$s = $dbh2->prepare("select col1 from test_pdo_oci_attr_autocommit_1");
$s->execute();
while ($r = $s->fetch()) {
    echo "Data is: " . $r[0] . "\n";
}

echo "Done\n";

?>
--CLEAN--
<?php
require 'ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory('ext/pdo_oci/tests/common.phpt');
$db->exec("DROP TABLE test_pdo_oci_attr_autocommit_1");
?>
--EXPECT--
PDO::ATTR_AUTOCOMMIT: Default: bool(true)
Change setting to false - PDO::ATTR_AUTOCOMMIT: bool(false)
Change setting back to true - PDO::ATTR_AUTOCOMMIT: bool(true)
Insert data
Second connection should be able to see committed data
Data is: some data
Done
