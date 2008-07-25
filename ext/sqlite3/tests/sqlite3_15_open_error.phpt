--TEST--
SQLite3::open error test
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
$unreadable = dirname(__FILE__) . '/unreadable.db';
touch($unreadable);
chmod($unreadable,  0200);
$db = new SQLite3($unreadable);

var_dump($db);
echo "Done\n";
unlink($unreadable);
?>
--EXPECTF--
Notice: SQLite3::__construct(): Unable to open database: unable to open database file in %s/sqlite3_15_open_error.php on line %d
object(SQLite3)#%d (0) {
}
Done
