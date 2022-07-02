--TEST--
SQLite3::changes (parameters) tests
--CREDITS--
Ward Hus
#@PHP TESTFEST 2009 (BELGIUM)
--EXTENSIONS--
sqlite3
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
