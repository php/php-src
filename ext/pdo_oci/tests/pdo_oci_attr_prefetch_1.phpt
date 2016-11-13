--TEST--
PDO_OCI: Attribute: Set prefetch on connection
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

echo "Test connect\n";
putenv('PDOTEST_ATTR='.serialize(array(PDO::ATTR_PREFETCH=>101)));
$dbh = PDOTest::factory();

echo $dbh->getAttribute(PDO::ATTR_PREFETCH), "\n";

// Verify can fetch
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Test set 102\n";
$dbh->setAttribute(PDO::ATTR_PREFETCH, 102);
echo $dbh->getAttribute(PDO::ATTR_PREFETCH), "\n";

// Verify can fetch
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Test set -1: (Uses 0)\n";
$dbh->setAttribute(PDO::ATTR_PREFETCH, -1);
echo $dbh->getAttribute(PDO::ATTR_PREFETCH), "\n";

// Verify can fetch
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Test set PHP_INT_MAX: (Uses default)\n";
$dbh->setAttribute(PDO::ATTR_PREFETCH, PHP_INT_MAX);
echo $dbh->getAttribute(PDO::ATTR_PREFETCH), "\n";

// Verify can fetch
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Done\n";

?>
--EXPECT--
Test connect
101
X
Test set 102
102
X
Test set -1: (Uses 0)
0
X
Test set PHP_INT_MAX: (Uses default)
100
X
Done
