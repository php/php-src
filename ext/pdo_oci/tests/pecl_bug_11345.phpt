--TEST--
PECL PDO_OCI Bug #11345 (Test invalid character set name)
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(__DIR__.'/../../pdo/tests/pdo_test.inc');
PDOTest::skip();
?>
--FILE--
<?php

// This tests only part of PECL bug 11345.  The other part - testing
// when the National Language Support (NLS) environment can't be
// initialized - is very difficult to test portably.

try {
    $dbh = new PDO('oci:dbname=xxx;charset=yyy', 'abc', 'def');
}
catch (PDOException $e) {
    echo 'Connection failed: ' . $e->getMessage(). "\n";
    exit;
}

echo "Done\n";

?>
--EXPECTF--
Connection failed: SQLSTATE[HY000]: OCINlsCharSetNameToId: unknown character set name (%s)
