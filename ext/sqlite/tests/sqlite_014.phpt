--TEST--
sqlite: fetch all (fetch_all)
--INI--
sqlite.assoc_case=0
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("sqlite")) print "skip"; ?>
--FILE--
<?php 
include "blankdb.inc";

$data = array(
	"one",
	"two",
	"three"
	);

sqlite_query("CREATE TABLE strings(a VARCHAR)", $db);

foreach ($data as $str) {
	sqlite_query("INSERT INTO strings VALUES('$str')", $db);
}

echo "unbuffered twice\n";
$r = sqlite_unbuffered_query("SELECT a from strings", $db, SQLITE_NUM);
var_dump(sqlite_fetch_all($r));
var_dump(sqlite_fetch_all($r));

echo "unbuffered with fetch_array\n";
$r = sqlite_unbuffered_query("SELECT a from strings", $db, SQLITE_NUM);
var_dump(sqlite_fetch_array($r));
var_dump(sqlite_fetch_all($r));

echo "buffered\n";
$r = sqlite_query("SELECT a from strings", $db, SQLITE_NUM);
var_dump(sqlite_fetch_all($r));
var_dump(sqlite_fetch_array($r));
var_dump(sqlite_fetch_all($r));

sqlite_close($db);

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

Warning: sqlite_fetch_all(): One or more rowsets were already returned; returning NULL this time in %ssqlite_014.php on line %d
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
