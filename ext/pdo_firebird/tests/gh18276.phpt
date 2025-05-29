--TEST--
GH-18276 (persistent connection - setAttribute(Pdo\Firebird::ATTR_DATE_FORMAT, ..) results in "zend_mm_heap corrupted")
--EXTENSIONS--
pdo_firebird
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
    $dbh->setAttribute(PDO::FB_ATTR_DATE_FORMAT, str_repeat('Y----m----d', random_int(1, 1)));
    $dbh->setAttribute(PDO::FB_ATTR_TIME_FORMAT, str_repeat('H::::i::::s', random_int(1, 1)));
    $dbh->setAttribute(PDO::FB_ATTR_TIMESTAMP_FORMAT, str_repeat('Y----m----d....H::::i::::s', random_int(1, 1)));
    unset($dbh);
}

echo 'done!';
?>
--EXPECT--
done!
