--TEST--
DBA DB4 with persistent connections
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

echo "Test 1\n";
$db_file1 = dba_popen($db_filename, 'n', 'flatfile');
dba_insert("key1", "This is a test insert 1", $db_file1);
echo dba_fetch("key1", $db_file1), "\n";


echo "Test 2\n";
$db_file2 = dba_popen($db_filename, 'n', 'flatfile');
if ($db_file1 === $db_file2) {
    echo "resources are the same\n";
} else {
    echo "resources are different\n";
}


echo "Test 3 - fetch both rows from second resource\n";
dba_insert("key2", "This is a test insert 2", $db_file2);
echo dba_fetch("key1", $db_file2), "\n";
echo dba_fetch("key2", $db_file2), "\n";


echo "Test 4 - fetch both rows from first resource\n";
echo dba_fetch("key1", $db_file1), "\n";
echo dba_fetch("key2", $db_file1), "\n";

echo "Test 5 - close 2nd resource\n";
dba_close($db_file2);
var_dump($db_file1);
var_dump($db_file2);

echo "Test 6 - query after closing 2nd resource\n";
echo dba_fetch("key1", $db_file1), "\n";
echo dba_fetch("key2", $db_file1), "\n";

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--XFAIL--
Test 6 crashes with dba pointer of NULL, bug http://bugs.php.net/bug.php?id=51278
--EXPECTF--
database handler: db4
Test 1
This is a test insert 1
Test 2
resources are different
Test 3 - fetch both rows from second resource
This is a test insert 1
This is a test insert 2
Test 4 - fetch both rows from first resource
This is a test insert 1
This is a test insert 2
Test 5 - close 2nd resource
resource(%d) of type (dba persistent)
resource(%d) of type (Unknown)
Test 6 - query after closing 2nd resource
This is a test insert 1
This is a test insert 2
