--TEST--
Bug #28112 (sqlite_query() crashing apache on malformed query)
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php

if (!($db = sqlite_open(":memory:", 666, $error))) die ("Couldn't open the database");
sqlite_query($db, "create table frob (foo INTEGER PRIMARY KEY, bar text);");
$res = @sqlite_array_query($db, "");

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===DONE===
