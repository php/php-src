--TEST--
PDO_Firebird: connect/disconnect
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
    unset($dbh);
    echo "done\n";

?>
--EXPECT--
done
