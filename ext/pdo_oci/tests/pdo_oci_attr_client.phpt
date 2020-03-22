--TEST--
PDO_OCI: Attribute: Client version
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

echo "ATTR_CLIENT_VERSION: ";
$cv = $dbh->getAttribute(PDO::ATTR_CLIENT_VERSION);
var_dump($cv);

$s = explode(".", $cv);
if (count($s) > 1 && (($s[0] == 10 && $s[1] >= 2) || $s[0] >= 11)) {
    if (count($s) != 5) {
        echo "Wrong number of values in array\nVersion was: ";
        var_dump($cv);
    } else {
        echo "Version OK, so far as can be portably checked\n";
    }
} else {
    if (count($s) != 2) {
        echo "Wrong number of values in array\nVersion was: ";
        var_dump($cv);
    } else {
        echo "Version OK, so far as can be portably checked\n";
    }
}

echo "Done\n";

?>
--EXPECTF--
ATTR_CLIENT_VERSION: string(%d) "%d.%s"
Version OK, so far as can be portably checked
Done
