--TEST--
DBA DB4 File open("rl") & Insert Test
--SKIPIF--
<?php 
$handler = "db4";
require_once(dirname(__FILE__) .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php

$handler = "db4";
require_once(dirname(__FILE__) .'/test.inc');
echo "database handler: $handler\n";

if (($db_file = dba_popen($db_filename, "c", $handler)) !== FALSE) {
    echo "database file created\n";
    dba_close($db_file);
}

if (($db_file = dba_popen($db_filename, "rl", $handler)) !== FALSE) {
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
require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECTF--
database handler: db4
database file created
database file opened

Warning: dba_insert(): You cannot perform a modification to a database without proper access in %sdba_db4_013.php on line %d
