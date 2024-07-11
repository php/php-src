--TEST--
PDO_Firebird: get api version
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
echo $dbh->getAttribute(PDO::FB_ATTR_API_VERSION) . "\n";
echo 'done!';
?>
--EXPECTF--
%d
done!
