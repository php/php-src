--TEST--
Test sqlite_popen() function : error conditions 
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip sqlite extension not loaded"; ?>
--FILE--
<?php
/* Prototype  : resource sqlite_popen(string filename [, int mode [, string &error_message]])
 * Description: Opens a persistent handle to a SQLite database. Will create the database if it does not exist. 
 * Source code: ext/sqlite/sqlite.c
 * Alias to functions: 
 */

$message = '';

echo "*** Testing sqlite_popen() : error conditions ***\n";

var_dump( sqlite_popen() );
var_dump( sqlite_popen(":memory:", 0666, $message, 'foobar') );
var_dump( sqlite_popen("", 0666, $message) );
var_dump( $message );

?>
===DONE===
--EXPECTF--
*** Testing sqlite_popen() : error conditions ***

Warning: sqlite_popen() expects at least 1 parameter, 0 given in %s on line %d
NULL

Warning: sqlite_popen() expects at most 3 parameters, 4 given in %s on line %d
NULL
bool(false)
NULL
===DONE===
