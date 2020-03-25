--TEST--
DBA with array key with empty array
--SKIPIF--
<?php
require_once(__DIR__ .'/skipif.inc');
die("info $HND handler used");
?>
--FILE--
<?php
require_once(__DIR__ .'/test.inc');
echo "database handler: $handler\n";

if (($db_file=dba_open($db_file, "n", $handler))!==FALSE) {
    dba_insert(array(), "Content String 1", $db_file);
} else {
    echo "Error creating database\n";
}

?>
--CLEAN--
<?php
require(__DIR__ .'/clean.inc');
?>
--EXPECTF--
database handler: %s

Fatal error: Uncaught Error: Key does not have exactly two elements: (key, name) in %sdba013.php:6
Stack trace:
#0 %sdba013.php(6): dba_insert(Array, '%s', Resource id #%d)
#1 {main}
  thrown in %sdba013.php on line 6
