--TEST--
SQLite: sqlite_popen() basic tests
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php
/* Prototype  : resource sqlite_popen(string filename [, int mode [, string &error_message]])
 * Description: Opens a persistent handle to a SQLite database. Will create the database if it does not exist.
 * Source code: ext/sqlite/sqlite.c
 * Alias to functions:
*/

	$db1 = sqlite_popen(":memory:");
	$db2 = sqlite_popen(":memory:");

	var_dump($db1);
	var_dump($db2);

	list($resourceId1) = sscanf((string) $db1, "resource(%d) of type (sqlite database (persistent))");
	list($resourceId2) = sscanf((string) $db2, "resource(%d) of type (sqlite database (persistent))");

	var_dump($resourceId1 === $resourceId2);
?>
--EXPECTF--
resource(%d) of type (sqlite database (persistent))
resource(%d) of type (sqlite database (persistent))
bool(true)
