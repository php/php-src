--TEST--
PDO_OCI: Attribute: verify driver name
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

require __DIR__ . '/../../pdo/tests/pdo_test.inc';

$dbh = PDOTest::factory();
var_dump($dbh->getAttribute(PDO::ATTR_DRIVER_NAME));

echo "Done\n";
?>
--EXPECT--
string(3) "oci"
Done
