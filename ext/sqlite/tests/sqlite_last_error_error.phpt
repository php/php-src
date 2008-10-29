--TEST--
Test sqlite_last_error() function : error conditions 
--SKIPIF--
<?php if (!extension_loaded("sqlite")) print "skip sqlite extension not loaded"; ?>
--FILE--
<?php
/* Prototype  : int sqlite_last_error(resource db)
 * Description: Returns the error code of the last error for a database. 
 * Source code: ext/sqlite/sqlite.c
 * Alias to functions: 
 */

echo "*** Testing sqlite_last_error() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing sqlite_last_error() function with Zero arguments --\n";
var_dump( sqlite_last_error() );

//Test sqlite_last_error with one more than the expected number of arguments
echo "\n-- Testing sqlite_last_error() function with more than expected no. of arguments --\n";

$db = sqlite_open(':memory:');
$extra_arg = 10;
var_dump( sqlite_last_error($db, $extra_arg) );
sqlite_close($db);

$db = new SQLiteDatabase(':memory:');
var_dump( $db->lastError($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing sqlite_last_error() : error conditions ***

-- Testing sqlite_last_error() function with Zero arguments --

Warning: sqlite_last_error() expects exactly 1 parameter, 0 given in %s on line %d
NULL

-- Testing sqlite_last_error() function with more than expected no. of arguments --

Warning: sqlite_last_error() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: SQLiteDatabase::lastError() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
