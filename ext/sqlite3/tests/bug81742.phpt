--TEST--
Bug #81742 (open_basedir bypass in SQLite3 by using url encoded file)
--EXTENSIONS--
sqlite3
--INI--
open_basedir=.
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->query("ATTACH 'file:..%2ffoo.php' as db2;");
?>
--EXPECTF--
Warning: SQLite3::query(): not authorized in %s on line %d
