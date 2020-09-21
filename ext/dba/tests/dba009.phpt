--TEST--
DBA dba_popen Test
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
    print("info $HND handler used");
?>
--FILE--
<?php
    require_once(__DIR__ .'/test.inc');
    echo "database handler: $handler\n";
    if (($db=dba_popen($db_file, "n", $handler))!==FALSE) {
        echo "Opened\n";
        dba_insert("a", "Inserted", $db);
        echo dba_fetch("a", $db)."\n";
        dba_close($db);
        echo "Closed\n";
    } else {
        echo "Error creating database\n";
    }
    if (($db=dba_popen($db_file, "n", $handler))!==FALSE) {
        echo "Opened\n";
        dba_insert("a", "Inserted", $db);
        echo dba_fetch("a", $db)."\n";
    }
?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: %s
Opened
Inserted
Closed
Opened
Inserted
