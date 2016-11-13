--TEST--
PDO_OCI: Attribute: verify driver name
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

require __DIR__ . '/../../pdo/tests/pdo_test.inc';

$dbh = PDOTest::factory();
var_dump($dbh->getAttribute(PDO::ATTR_DRIVER_NAME));

echo "Done\n";
?>
--EXPECT--
string(3) "oci"
Done
