--TEST--
sqlite-oo: calling static methods
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; 
?>
--FILE--
<?php

require_once('blankdb_oo.inc'); 

class foo {
    static function bar($param = NULL) {
		return $param;
    }
}

function baz($param = NULL) {
	return $param;
}

var_dump($db->single_query("select php('baz')", 1));
var_dump($db->single_query("select php('baz', 1)", 1));
var_dump($db->single_query("select php('baz', \"PHP\")", 1));
var_dump($db->single_query("select php('foo::bar')", 1));
var_dump($db->single_query("select php('foo::bar', 1)", 1));
var_dump($db->single_query("select php('foo::bar', \"PHP\")", 1));
var_dump($db->single_query("select php('foo::bar(\"PHP\")')", 1));

?>
===DONE===
--EXPECTF--
NULL
string(1) "1"
string(3) "PHP"
NULL
string(1) "1"
string(3) "PHP"

Warning: sqlite_db::single_query(): call_user_function_ex failed for function foo::bar("PHP")() in %ssqlite_oo_030.php on line %d
bool(false)
===DONE===
