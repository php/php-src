--TEST--
PDO_OCI: Attribute: prefetch on statements
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require(dirname(__FILE__) . '/../../pdo/tests/pdo_test.inc');

$dbh = PDOTest::factory();

$s = $dbh->prepare("select '' as myempty, null as mynull from dual", array(PDO::ATTR_PREFETCH => 101));

echo "Test 1: Can't set prefetch after prepare\n";
var_dump($s->setAttribute(PDO::ATTR_PREFETCH, 102));

// Verify can fetch
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Test 2: Turn off prefetching\n";
$s = $dbh->prepare("select '' as myempty, null as mynull from dual", array(PDO::ATTR_PREFETCH => 0));
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Done\n";

?>
--EXPECTF--
Test 1: Can't set prefetch after prepare

Warning: PDOStatement::setAttribute(): SQLSTATE[IM001]: Driver does not support this function: This driver doesn't support setting attributes in %s on line %d
bool(false)
X
Test 2: Turn off prefetching
X
Done
