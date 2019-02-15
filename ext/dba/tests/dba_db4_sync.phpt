--TEST--
DBA DB4 Sync Test
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
if (($db_file=dba_open($db_filename, "n", $handler))!==FALSE) {
    dba_insert("key1", "Content String 1", $db_file);
    dba_insert("key2", "Content String 2", $db_file);
    for ($i=1; $i<3; $i++) {
        echo dba_exists("key$i", $db_file) ? "Y" : "N";
    }
    echo "\n";
    var_dump(dba_sync($db_file));
    dba_close($db_file);
} else {
    echo "Error creating database\n";
}

?>
===DONE===
<?php exit(0); ?>
--CLEAN--
<?php
require(dirname(__FILE__) .'/clean.inc');
?>
--EXPECT--
database handler: db4
YY
bool(true)
===DONE===
