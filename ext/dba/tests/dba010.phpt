--TEST--
DBA with array keys
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
    dba_insert(array("", "name0") , "Content String 1", $db_file);
    dba_insert(array("key1", "name1") , "Content String 1", $db_file);
    dba_insert(array("key2","name2"), "Content String 2", $db_file);
    dba_insert("[key3]name3", "Third Content String", $db_file);
    dba_insert(array("key4","name4"), "Another Content String", $db_file);
    dba_insert(array("key5","name5"), "The last content string", $db_file);
    $a = dba_firstkey($db_file);
    $i=0;
    while($a) {
        $a = dba_nextkey($db_file);
        $i++;
    }
    echo $i;
    echo dba_exists(array("","name0"), $db_file) ? "Y" : "N";
    for ($i=1; $i<5; $i++) {
        echo dba_exists("[key$i]name$i", $db_file) ? "Y" : "N";
    }
    echo dba_exists(array("key5","name5"), $db_file) ? "Y" : "N";
    echo "\n";
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
database handler: %s
6YYYYYY
