--TEST--
PDO_Firebird: connect/disconnect
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
    require("testdb.inc");

    $dbh = getDbConnection();
    unset($dbh);
    echo "done\n";

?>
--EXPECT--
done
