--TEST--
DBA with array key with empty array
--SKIPIF--
<?php 
require_once(dirname(__FILE__) .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
require_once(dirname(__FILE__) .'/test.inc');
echo "database handler: $handler\n";

if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    dba_insert(array(), "Content String 1", $db_file);
} else {
    echo "Error creating database\n";
}

?>
--CLEAN--
<?php 
require(dirname(__FILE__) .'/clean.inc'); 
?>
--EXPECTF--
database handler: %s

Catchable fatal error: dba_insert(): Key does not have exactly two elements: (key, name) in %sdba013.php on line %d
