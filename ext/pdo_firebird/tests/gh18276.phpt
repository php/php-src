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

$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
    [
        PDO::ATTR_PERSISTENT => true,
    ],
);
unset($dbh);

$dbh = new PDO(
    PDO_FIREBIRD_TEST_DSN,
    PDO_FIREBIRD_TEST_USER,
    PDO_FIREBIRD_TEST_PASS,
    [
        PDO::ATTR_PERSISTENT => true,
    ],
);
$dbh->setAttribute(Pdo\firebird::ATTR_DATE_FORMAT, 'Y----m----d');
$dbh->setAttribute(Pdo\firebird::ATTR_TIME_FORMAT, 'H::::i::::s');
$dbh->setAttribute(Pdo\firebird::ATTR_TIMESTAMP_FORMAT, 'Y----m----d....H::::i::::s');

echo 'done!';
?>
--EXPECT--
done!
