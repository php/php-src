--TEST--
sqlite-spl: Countable
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
if (!extension_loaded("spl")) print "skip SPL is not present"; 
?>
--FILE--
<?php
include "blankdb_oo.inc";

$db->query("CREATE TABLE menu(id_l int PRIMARY KEY, id_r int UNIQUE, key VARCHAR(10))");
$db->query("INSERT INTO menu VALUES( 1, 12, 'A')"); 
$db->query("INSERT INTO menu VALUES( 2,  9, 'B')"); 
$db->query("INSERT INTO menu VALUES(10, 11, 'F')"); 
$db->query("INSERT INTO menu VALUES( 3,  6, 'C')"); 
$db->query("INSERT INTO menu VALUES( 7,  8, 'E')"); 
$db->query("INSERT INTO menu VALUES( 4,  5, 'D')"); 

$res = $db->query("SELECT * from menu");

var_dump($res->count());
var_dump(count($res));
?>
===DONE===
--EXPECT--
int(6)
int(6)
===DONE===
