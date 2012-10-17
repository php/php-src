--TEST--
sqlite-oo: sqlite_open/close
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
require_once('blankdb_oo.inc');
var_dump($db);
$db = NULL;
echo "Done\n";
?>
--EXPECTF--
object(SQLiteDatabase)#%d (0) {
}
Done
