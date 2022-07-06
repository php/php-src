--TEST--
DBA DB4 File Creation open("c") with existing file
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

var_dump(file_put_contents($db_filename, "Dummy contents"));

if (($db_file = dba_open($db_filename, "c", $handler)) !== FALSE) {
    if (file_exists($db_filename)) {
        echo "database file created\n";
        dba_close($db_file);
    } else {
        echo "File did not get created\n";
    }
} else {
    echo "Error creating $db_filename\n";
}

// Check the file still exists
$s = file_get_contents($db_filename);
echo "$s\n";

?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: db4
int(14)

Warning: dba_open(%stest0.dbm,c): Driver initialization failed for handler: db4: Invalid argument in %sdba_db4_003.php on line %d
Error creating %stest0.dbm
Dummy contents
