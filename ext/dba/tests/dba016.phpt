--TEST--
DBA lock modifier error message test
--SKIPIF--
<?php
$handler = "flatfile";
require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php

$handler = "flatfile";
require_once(__DIR__ .'/test.inc');
echo "database handler: $handler\n";

$db_file1 = dba_popen($db_filename, 'n-t', 'flatfile');

?>
===DONE===
--CLEAN--
<?php
	require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: flatfile

Warning: dba_popen(%stest0.dbm,n-t): You cannot combine modifiers - (no lock) and t (test lock) in %sdba016.php on line %d
===DONE===
