--TEST--
SQLite3::open/close tests
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

$db_file = __DIR__ . DIRECTORY_SEPARATOR . '私はガラスを食べられます.db';
$db = new SQLite3($db_file);
//require_once(__DIR__ . '/new_db.inc');

var_dump($db);
var_dump($db->close());

unlink($db_file);
echo "Done\n";
?>
--EXPECTF--
object(SQLite3)#%d (0) {
}
bool(true)
Done
