--TEST--
sqlite: sqlite_[has_]prev
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

sqlite_query("CREATE TABLE strings(a)", $db);

foreach ($data as $str) {
  sqlite_query("INSERT INTO strings VALUES('$str')", $db);
}

$r = sqlite_query("SELECT a FROM strings", $db, SQLITE_NUM);

echo "====TRAVERSE====\n";
for(sqlite_rewind($r); sqlite_valid($r); sqlite_next($r)) {
  var_dump(sqlite_current($r));

}
echo "====REVERSE====\n";
do {
  sqlite_prev($r);
  var_dump(sqlite_current($r));
} while(sqlite_has_prev($r));

echo "====UNBUFFERED====\n";

$r = sqlite_unbuffered_query("SELECT a FROM strings", $db, SQLITE_NUM);

echo "====TRAVERSE====\n";
for(sqlite_rewind($r); sqlite_valid($r); sqlite_next($r)) {
  var_dump(sqlite_current($r));

}
echo "====REVERSE====\n";
do {
  sqlite_prev($r);
  var_dump(sqlite_current($r));
} while(sqlite_has_prev($r));

sqlite_close($db);

echo "====DONE!====\n";
?>
--EXPECTF--
====TRAVERSE====
array(1) {
  [0]=>
  string(3) "one"
}
array(1) {
  [0]=>
  string(3) "two"
}
array(1) {
  [0]=>
  string(5) "three"
}
====REVERSE====
array(1) {
  [0]=>
  string(5) "three"
}
array(1) {
  [0]=>
  string(3) "two"
}
array(1) {
  [0]=>
  string(3) "one"
}
====UNBUFFERED====
====TRAVERSE====

Warning: sqlite_rewind(): Cannot rewind an unbuffered result set in %ssqlite_023.php on line %d
array(1) {
  [0]=>
  string(3) "one"
}
array(1) {
  [0]=>
  string(3) "two"
}
array(1) {
  [0]=>
  string(5) "three"
}
====REVERSE====

Warning: sqlite_prev(): you cannot use sqlite_prev on unbuffered querys in %ssqlite_023.php on line %d
bool(false)

Warning: sqlite_has_prev(): you cannot use sqlite_has_prev on unbuffered querys in %ssqlite_023.php on line %d
====DONE!====
