--TEST--
PDO_Firebird: attr date, time, and timestamp formats
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
$dbh = getDbConnection();

echo "== Default state with nothing set. ==\n";
echo 'ATTR_DATE_FORMAT: ', $dbh->getAttribute(Pdo\firebird::ATTR_DATE_FORMAT), "\n";
echo 'ATTR_TIME_FORMAT: ', $dbh->getAttribute(Pdo\firebird::ATTR_TIME_FORMAT), "\n";
echo 'ATTR_TIMESTAMP_FORMAT: ', $dbh->getAttribute(Pdo\firebird::ATTR_TIMESTAMP_FORMAT), "\n";

$dbh->setAttribute(Pdo\firebird::ATTR_DATE_FORMAT, 'Y----m----d');
$dbh->setAttribute(Pdo\firebird::ATTR_TIME_FORMAT, 'H::::i::::s');
$dbh->setAttribute(Pdo\firebird::ATTR_TIMESTAMP_FORMAT, 'Y----m----d....H::::i::::s');

echo "\n";

echo "== State after setting value. ==\n";
echo 'ATTR_DATE_FORMAT: ', $dbh->getAttribute(Pdo\firebird::ATTR_DATE_FORMAT), "\n";
echo 'ATTR_TIME_FORMAT: ', $dbh->getAttribute(Pdo\firebird::ATTR_TIME_FORMAT), "\n";
echo 'ATTR_TIMESTAMP_FORMAT: ', $dbh->getAttribute(Pdo\firebird::ATTR_TIMESTAMP_FORMAT), "\n";
?>
--EXPECT--
== Default state with nothing set. ==
ATTR_DATE_FORMAT: %Y-%m-%d
ATTR_TIME_FORMAT: %H:%M:%S
ATTR_TIMESTAMP_FORMAT: %Y-%m-%d %H:%M:%S

== State after setting value. ==
ATTR_DATE_FORMAT: Y----m----d
ATTR_TIME_FORMAT: H::::i::::s
ATTR_TIMESTAMP_FORMAT: Y----m----d....H::::i::::s
