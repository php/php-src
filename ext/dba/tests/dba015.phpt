--TEST--
DBA with persistent connections
--EXTENSIONS--
dba
--SKIPIF--
<?php
$handler = "flatfile";
require_once(__DIR__ .'/skipif.inc');
die("info $HND handler used");
?>
--CONFLICTS--
dba
--FILE--
<?php

$handler = "flatfile";
require_once(__DIR__ .'/test.inc');
echo "database handler: $handler\n";

echo "Test 1\n";
$db_file1 = dba_popen($db_filename, 'n', 'flatfile');
dba_insert("key1", "This is a test insert 1", $db_file1);
echo dba_fetch("key1", $db_file1), "\n";


echo "Test 2\n";
$db_file2 = dba_popen($db_filename, 'n', 'flatfile');
if ($db_file1 === $db_file2) {
    echo "objects are the same\n";
} else {
    echo "objects are different\n";
}


echo "Test 3 - fetch both rows from second object\n";
dba_insert("key2", "This is a test insert 2", $db_file2);
echo dba_fetch("key1", $db_file2), "\n";
echo dba_fetch("key2", $db_file2), "\n";


echo "Test 4 - fetch both rows from first object\n";
echo dba_fetch("key1", $db_file1), "\n";
echo dba_fetch("key2", $db_file1), "\n";

echo "Test 5 - close 2nd object\n";
dba_close($db_file2);
var_dump($db_file1);
try {
    dba_exists("key1", $db_file2);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

echo "Test 6 - query after closing 2nd object\n";
echo dba_fetch("key1", $db_file1), "\n";
echo dba_fetch("key2", $db_file1), "\n";

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: flatfile
Test 1
This is a test insert 1
Test 2
objects are different
Test 3 - fetch both rows from second object
This is a test insert 1
This is a test insert 2
Test 4 - fetch both rows from first object
This is a test insert 1
This is a test insert 2
Test 5 - close 2nd object
object(Dba\Connection)#%d (%d) {
}
DBA connection has already been closed
Test 6 - query after closing 2nd object
This is a test insert 1
This is a test insert 2
