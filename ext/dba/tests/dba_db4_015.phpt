--TEST--
DBA DB4 File open("wl") & Insert Test
--SKIPIF--
<?php
$handler = "db4";
require_once(__DIR__ .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php

$handler = "db4";
require_once(__DIR__ .'/test.inc');
echo "database handler: $handler\n";

if (($db_file = dba_popen($db_filename, "c", $handler)) !== FALSE) {
    echo "database file created\n";
    dba_close($db_file);
}

if (($db_file = dba_popen($db_filename, "wl", $handler)) !== FALSE) {
    echo "database file opened\n";
    dba_insert("key1", "This is a test insert", $db_file);
    echo dba_fetch("key1", $db_file), "\n";
    dba_close($db_file);
} else {
    echo "Error creating $db_filename\n";
}

?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECT--
database handler: db4
database file created
database file opened
This is a test insert
