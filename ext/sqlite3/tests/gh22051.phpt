--TEST--
GH-22051 (SQLite3Result::reset()/finalize() report the error on failure)
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');

// A query whose second step overflows leaves the statement in an error state.
$sql = "SELECT abs(x) FROM (SELECT 1 AS x UNION ALL SELECT -9223372036854775807 - 1)";

echo "--- SQLite3Result::reset() ---\n";
$result = $db->query($sql);
var_dump($result->fetchArray(SQLITE3_NUM));
var_dump(@$result->fetchArray(SQLITE3_NUM));
var_dump($result->reset());

echo "--- SQLite3Result::finalize() ---\n";
$stmt = $db->prepare($sql);
$result = $stmt->execute();
var_dump($result->fetchArray(SQLITE3_NUM));
var_dump(@$result->fetchArray(SQLITE3_NUM));
var_dump($result->finalize());
?>
--EXPECTF--
--- SQLite3Result::reset() ---
array(1) {
  [0]=>
  int(1)
}
bool(false)

Warning: SQLite3Result::reset(): Unable to reset statement: integer overflow in %s on line %d
bool(false)
--- SQLite3Result::finalize() ---
array(1) {
  [0]=>
  int(1)
}
bool(false)

Warning: SQLite3Result::finalize(): Unable to reset statement: integer overflow in %s on line %d
bool(true)
