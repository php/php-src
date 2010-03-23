--TEST--
sqlite-oo: single query
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$db->query("CREATE TABLE test_db ( id INTEGER PRIMARY KEY, data VARCHAR(100) )");
for ($i = 0; $i < 10; $i++) {
	$db->query("INSERT INTO test_db (data) VALUES('{$i}data')");
}
$db->query("INSERT INTO test_db (data) VALUES(NULL)");

var_dump($db->singleQuery("SELECT id FROM test_db WHERE id=5"));
var_dump($db->singleQuery("SELECT * FROM test_db WHERE id=4"));
var_dump($db->singleQuery("SELECT data FROM test_db WHERE id=6"));
var_dump($db->singleQuery("SELECT * FROM test_db WHERE id < 5"));
var_dump($db->singleQuery("SELECT * FROM test db WHERE id < 4"));
var_dump($db->singleQuery("SELECT * FROM test_db WHERE id=999999"));
var_dump($db->singleQuery("SELECT id FROM test_db WHERE id=5", FALSE));

echo "DONE!\n";
?>
--EXPECTF--
string(1) "5"
string(1) "4"
string(5) "5data"
array(4) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
  [2]=>
  string(1) "3"
  [3]=>
  string(1) "4"
}

Warning: SQLiteDatabase::singleQuery(): no such table: test in %s on line %d
bool(false)
NULL
array(1) {
  [0]=>
  string(1) "5"
}
DONE!
