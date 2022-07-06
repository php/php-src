--TEST--
PDO_OCI: Attribute: closing a connection in non-autocommit mode commits data
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(__DIR__ . '/../../pdo/tests/pdo_test.inc');

// Check connection can be created with AUTOCOMMIT off
putenv('PDOTEST_ATTR='.serialize(array(PDO::ATTR_AUTOCOMMIT=>false)));
$dbh = PDOTest::factory();

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);
$dbh->exec("drop table pdo_ac_tab");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

print "PDO::ATTR_AUTOCOMMIT: ";
var_dump($dbh->getAttribute(PDO::ATTR_AUTOCOMMIT));

echo "Insert data\n";

$dbh->exec("create table pdo_ac_tab (col1 varchar2(20))");

$dbh->exec("insert into pdo_ac_tab (col1) values ('some data')");

$dbh = null; // close first connection

echo "Second connection should be able to see committed data\n";
$dbh2 = PDOTest::factory();
$s = $dbh2->prepare("select col1 from pdo_ac_tab");
$s->execute();
while ($r = $s->fetch()) {
    echo "Data is: " . $r[0] . "\n";
}

$dbh2->exec("drop table pdo_ac_tab");

echo "Done\n";

?>
--EXPECT--
PDO::ATTR_AUTOCOMMIT: bool(false)
Insert data
Second connection should be able to see committed data
Done
