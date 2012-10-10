--TEST--
sqlite: crash on bad queries inside sqlite_array_query()
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php
include "blankdb.inc";

sqlite_array_query($db, "SELECT foo FROM foobar");
sqlite_close($db);
?>
--EXPECTF--
Warning: sqlite_array_query(): no such table: foobar in %s on line %d
