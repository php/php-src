--TEST--
sqlite-oo: fetch all
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb_oo.inc";

$data = array(
	"one",
	"two",
	"three"
	);

$db->query("CREATE TABLE strings(a VARCHAR)");

foreach ($data as $str) {
	$db->query("INSERT INTO strings VALUES('$str')");
}

echo "unbuffered twice\n";
$r = $db->unbuffered_query("SELECT a from strings", SQLITE_NUM);
var_dump($r->fetch_all());
var_dump($r->fetch_all());

echo "unbuffered with fetch_array\n";
$r = $db->unbuffered_query("SELECT a from strings", SQLITE_NUM);
var_dump($r->fetch_array());
var_dump($r->fetch_all());

echo "buffered\n";
$r = $db->query("SELECT a from strings", SQLITE_NUM);
var_dump($r->fetch_all());
var_dump($r->fetch_array());
var_dump($r->fetch_all());

echo "DONE!\n";
?>
--EXPECTF--
unbuffered twice
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "one"
  }
  [1]=>
  array(1) {
    [0]=>
    string(3) "two"
  }
  [2]=>
  array(1) {
    [0]=>
    string(5) "three"
  }
}

Notice: sqlite_ub_query::fetch_all(): One or more rowsets were already returned in %ssqlite_oo_014.php on line %d
array(0) {
}
unbuffered with fetch_array
array(1) {
  [0]=>
  string(3) "one"
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "two"
  }
  [1]=>
  array(1) {
    [0]=>
    string(5) "three"
  }
}
buffered
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "one"
  }
  [1]=>
  array(1) {
    [0]=>
    string(3) "two"
  }
  [2]=>
  array(1) {
    [0]=>
    string(5) "three"
  }
}
bool(false)
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(3) "one"
  }
  [1]=>
  array(1) {
    [0]=>
    string(3) "two"
  }
  [2]=>
  array(1) {
    [0]=>
    string(5) "three"
  }
}
DONE!
