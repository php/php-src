--TEST--
Test sqlite_exec() function : basic functionality 
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip sqlite extension not loaded"; ?>
--FILE--
<?php
/* Prototype  : boolean sqlite_exec(string query, resource db[, string &error_message])
 * Description: Executes a result-less query against a given database 
 * Source code: ext/sqlite/sqlite.c
 * Alias to functions: 
 */

echo "*** Testing sqlite_exec() : basic functionality ***\n";

// set up variables
$query = 'CREATE TABLE foobar (id INTEGER PRIMARY KEY, name CHAR(255));';
$error_message = null;

// procedural
$db = sqlite_open(':memory:');
var_dump( sqlite_exec($db, $query) );
sqlite_close($db);

// oo-style
$db = new SQLiteDatabase(':memory:');
var_dump( $db->queryExec($query, $error_message) );

?>
===DONE===
--EXPECTF--
*** Testing sqlite_exec() : basic functionality ***
bool(true)
bool(true)
===DONE===
