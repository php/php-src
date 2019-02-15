--TEST--
DBA DB4 File Creation popen("c") with existing valid file
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
    if (file_exists($db_filename)) {
        echo "database file created\n";
        var_dump(dba_insert("key1", "This is a test insert", $db_file));
        echo dba_fetch("key1", $db_file), "\n";
        dba_close($db_file);
    } else {
        echo "File did not get created\n";
    }
} else {
    echo "Error creating $db_filename\n";
}

// Now test reopening it
if (($db_file = dba_popen($db_filename, "c", $handler)) !== FALSE) {
    if (file_exists($db_filename)) {
        echo "database file created\n";
        var_dump(dba_insert("key1", "second open test", $db_file));
        var_dump(dba_insert("key2", "second open test row 2", $db_file));
        echo dba_fetch("key1", $db_file), "\n";
        echo dba_fetch("key2", $db_file), "\n";
        dba_close($db_file);
    } else {
        echo "File did not get created\n";
    }
} else {
    echo "Error creating $db_filename\n";
}

?>
--CLEAN--
<?php
require(dirname(__FILE__) .'/clean.inc');
?>
--EXPECT--
database handler: db4
database file created
bool(true)
This is a test insert
database file created
bool(false)
bool(true)
This is a test insert
second open test row 2
