--TEST--
SQLite3::changes (parameters) tests
--CREDITS--
Ward Hus
#@PHP TESTFEST 2009 (BELGIUM)
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
$db = new SQLite3(':memory:');
var_dump($db);
var_dump($db->changes());
echo "Done\n";
?>
--EXPECT--
object(SQLite3)#1 (0) {
}
int(0)
Done
