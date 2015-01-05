--TEST--
PDO_OCI: Attribute: Set prefetch on connection
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

echo "Test connect: (value is ignored & has no effect)\n";
putenv('PDOTEST_ATTR='.serialize(array(PDO::ATTR_PREFETCH=>101)));
$dbh = PDOTest::factory();

echo "Test set: (value is ignored & has no effect)\n";
$dbh->setAttribute(PDO::ATTR_PREFETCH, 102);

// Verify can fetch
$s = $dbh->prepare("select dummy from dual" );
$s->execute();
while ($r = $s->fetch()) {
    echo $r[0] . "\n";
}

echo "Done\n";

?>
--EXPECT--
Test connect: (value is ignored & has no effect)
Test set: (value is ignored & has no effect)
X
Done
