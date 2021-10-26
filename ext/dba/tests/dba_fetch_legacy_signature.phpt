--TEST--
dba_fetch() legacy signature
--EXTENSIONS--
dba
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
    die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
$db_file = dba_open($db_file, "n", $handler);

if ($db_file === false) {
    die('Error creating database');
}

dba_insert("key1", "This is a test insert", $db_file);
echo dba_fetch("key1", 0, $db_file), \PHP_EOL, dba_fetch("key1", $db_file, 0);
dba_close($db_file);

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECT--
This is a test insert
This is a test insert
