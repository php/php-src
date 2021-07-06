--TEST--
PDO_Firebird: connect/disconnect
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
    require("testdb.inc");

    unset($dbh);
    echo "done\n";

?>
--EXPECT--
done
