--TEST--
Test sqlite_last_error() function : basic functionality 
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip sqlite extension not loaded"; ?>
--FILE--
<?php
/* Prototype  : int sqlite_last_error(resource db)
 * Description: Returns the error code of the last error for a database. 
 * Source code: ext/sqlite/sqlite.c
 * Alias to functions: 
 */

echo "*** Testing sqlite_last_error() : basic functionality ***\n";

// set up variables
$query = 'CREATE TAB LE foobar (id INTEGER PRIMARY KEY, name CHAR(255));';
$query_ok = 'CREATE TABLE foobar (id INTEGER, name CHAR(255));';

// procedural
$db = sqlite_open(':memory:');
var_dump( sqlite_last_error($db) === SQLITE_OK );
sqlite_exec($db, $query);
var_dump( sqlite_last_error($db) === SQLITE_ERROR );
sqlite_exec($db, $query_ok);
var_dump( sqlite_last_error($db) === SQLITE_OK );
sqlite_close($db);

// oo-style
$db = new SQLiteDatabase(':memory:');
$db->queryExec($query);
var_dump( $db->lastError() === SQLITE_ERROR );
$db->queryExec($query_ok);
var_dump( $db->lastError() === SQLITE_OK );

?>
===DONE===
--EXPECTF--
*** Testing sqlite_last_error() : basic functionality ***
bool(true)

Warning: sqlite_exec(): near "TAB": syntax error in %s on line %d
bool(true)
bool(true)

Warning: SQLiteDatabase::queryExec(): near "TAB": syntax error in %s on line %d
bool(true)
bool(true)
===DONE===
