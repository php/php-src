--TEST--
sqlite: sqlite_fetch_object
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

class class24 {
	function __construct() {
		echo __METHOD__ . "\n";
	}
}

$data = array(
	"one",
	"two",
	"three"
	);

sqlite_query($db, "CREATE TABLE strings(a)");

foreach ($data as $str) {
	sqlite_query($db, "INSERT INTO strings VALUES('$str')");
}

echo "====class24====\n";
$res = sqlite_query($db, "SELECT a FROM strings", SQLITE_ASSOC);
while (sqlite_valid($res)) {
	var_dump(sqlite_fetch_object($res, 'class24'));
}

echo "====stdclass====\n";
$res = sqlite_query($db, "SELECT a FROM strings", SQLITE_ASSOC);
while (sqlite_valid($res)) {
	var_dump(sqlite_fetch_object($res));
}

sqlite_close($db);

echo "====DONE!====\n";
?>
--EXPECTF--
====class24====
class24::__construct
object(class24)#%d (1) {
  ["a"]=>
  string(3) "one"
}
class24::__construct
object(class24)#%d (1) {
  ["a"]=>
  string(3) "two"
}
class24::__construct
object(class24)#%d (1) {
  ["a"]=>
  string(5) "three"
}
====stdclass====
object(stdClass)#%d (1) {
  ["a"]=>
  string(3) "one"
}
object(stdClass)#%d (1) {
  ["a"]=>
  string(3) "two"
}
object(stdClass)#%d (1) {
  ["a"]=>
  string(5) "three"
}
====DONE!====
