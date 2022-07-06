--TEST--
Bug #65708 (dba functions cast $key param to string in-place, bypassing copy on write)
--SKIPIF--
<?php
    require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php

error_reporting(E_ALL);

require_once(__DIR__ .'/test.inc');

$db = dba_popen($db_filename, 'c');

$key = 1;
$copy = $key;

echo gettype($key)."\n";
echo gettype($copy)."\n";

dba_exists($key, $db);

echo gettype($key)."\n";
echo gettype($copy)."\n";

dba_close($db);

?>
--CLEAN--
<?php
    require(__DIR__ .'/clean.inc');
?>
--EXPECT--
integer
integer
integer
integer
