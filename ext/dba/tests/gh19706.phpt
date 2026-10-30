--TEST--
GH-19706 (dba stream resource mismanagement)
--EXTENSIONS--
dba
--FILE--
<?php
// Must be in the global scope such that it's part of the symbol table cleanup
$db = dba_open(__DIR__ . '/gh19706.cdb', 'n', 'cdb_make');
$db2 = $db;
var_dump($db, $db2);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh19706.cdb');
?>
--EXPECT--
object(Dba\Connection)#1 (0) {
}
object(Dba\Connection)#1 (0) {
}
