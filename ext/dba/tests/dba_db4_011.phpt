--TEST--
DBA DB4 with repeated key
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
if (($db_file=dba_open($db_filename, "c", $handler))!==FALSE) {
    var_dump(dba_insert("key1", "Content String 1", $db_file));
    var_dump(dba_insert("key2", "Content String 2", $db_file));
    var_dump(dba_insert("key2", "Same key", $db_file));
    echo dba_fetch("key1", $db_file), "\n";
    echo dba_fetch("key2", $db_file), "\n";
    dba_close($db_file);
} else {
    echo "Error creating database\n";
}

?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECT--
database handler: db4
bool(true)
bool(true)
bool(false)
Content String 1
Content String 2
