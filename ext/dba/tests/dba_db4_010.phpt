--TEST--
DBA DB4 Quote Test
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
if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    dba_insert("key1", '"', $db_file);
    var_dump(dba_fetch("key1", $db_file));
    dba_replace("key1", '\"', $db_file);
    var_dump(dba_fetch("key1", $db_file));
    dba_close($db_file);
} else {
    echo "Error creating database\n";
}
?>
--CLEAN--
<?php 
require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECTF--
database handler: db4
string(1) """
string(2) "\""
