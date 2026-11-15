--TEST--
GH-18276 (persistent connection - setAttribute(Pdo\Firebird::ATTR_DATE_FORMAT, ..) results in "zend_mm_heap corrupted")
--EXTENSIONS--
pdo_firebird
zend_test
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require("testdb.inc");
unset($dbh);

for ($i = 0; $i < 2; $i++) {
    $dbh = new PDO(
        PDO_FIREBIRD_TEST_DSN,
        PDO_FIREBIRD_TEST_USER,
        PDO_FIREBIRD_TEST_PASS,
        [
            PDO::ATTR_PERSISTENT => true,
        ],
    );
    // Avoid interned
    $dbh->setAttribute(Pdo\Firebird::ATTR_DATE_FORMAT, zend_return_non_interned_string('Y----m----d'));
    $dbh->setAttribute(Pdo\Firebird::ATTR_TIME_FORMAT, zend_return_non_interned_string('H::::i::::s'));
    $dbh->setAttribute(Pdo\Firebird::ATTR_TIMESTAMP_FORMAT, zend_return_non_interned_string('Y----m----d....H::::i::::s'));
    unset($dbh);
}

echo 'done!';
?>
--EXPECT--
done!
