--TEST--
Test sqlite_exec() function : error behaviour and functionality 
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip sqlite extension not loaded"; ?>
--FILE--
<?php
/* Prototype  : boolean sqlite_exec(string query, resource db[, string &error_message])
 * Description: Executes a result-less query against a given database 
 * Source code: ext/sqlite/sqlite.c
 * Alias to functions: 
 */

echo "*** Testing sqlite_exec() : error functionality ***\n";

// set up variables
$fail = 'CRE ATE TABLE';
$error_message = null;

// procedural
$db = sqlite_open(':memory:');
var_dump( sqlite_exec($db, $fail, $error_message) );
var_dump( $error_message );
var_dump( sqlite_exec($db) );
sqlite_close($db);

// oo-style
$db = new SQLiteDatabase(':memory:');
var_dump( $db->queryExec($fail, $error_message, 'fooparam') );

?>
===DONE===
--EXPECTF--
*** Testing sqlite_exec() : error functionality ***

Warning: sqlite_exec(): near "CRE": syntax error in %s on line %d
bool(false)
%string|unicode%(24) "near "CRE": syntax error"

Warning: sqlite_exec() expects at least 2 parameters, 1 given in %s on line %d
NULL

Warning: SQLiteDatabase::queryExec() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===
