--TEST--
DBA Sync Test
--SKIPIF--
--FILE--
<?php
require_once(dirname(__FILE__) .'/test.inc');
echo "database handler: $handler\n";
if (($db_file=dba_open($db_filename, "n", $handler))!==FALSE) {
    dba_insert("key1", "Content String 1", $db_file);
    dba_insert("key2", "Content String 2", $db_file);
    $a = dba_firstkey($db_file);
    $i=0;
    while($a) {
        $a = dba_nextkey($db_file);
        $i++;
    }
    echo $i;
    for ($i=1; $i<3; $i++) {
        echo dba_exists("key$i", $db_file) ? "Y" : "N";
    }
    echo "\n";
    var_dump(dba_sync());
    var_dump(dba_sync(""));
    var_dump(dba_sync($db_file));
    dba_close($db_file);
} else {
    echo "Error creating database\n";
}

require_once(dirname(__FILE__) .'/clean.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
database handler: flatfile
2YY

Warning: Wrong parameter count for dba_sync() in %sdba_sync.php on line %d
NULL

Warning: dba_sync(): supplied argument is not a valid DBA identifier resource in %sdba_sync.php on line %d
bool(false)
bool(true)
===DONE===
