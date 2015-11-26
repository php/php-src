--TEST--
DBA DB4 New File Creation open("rl")
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
if (($db_file = dba_open($db_filename, "rl", $handler)) !== FALSE) {
    echo "database file created\n";
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

Warning: dba_open(%stest0.dbm,rl): Driver initialization failed for handler: db4: No such file or directory in %sdba_db4_012.php on line %d
Error creating %stest0.dbm
