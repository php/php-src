--TEST--
DBA DB4 Sync Test
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
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECT--
database handler: db4
YY
bool(true)
