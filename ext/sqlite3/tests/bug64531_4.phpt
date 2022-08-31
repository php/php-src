--TEST--
Bug #64531 (SQLite3Result::fetchArray runs the query again) - SQLite3Stmt->execute (reused)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip sqlite3 extension not available');
?>
--FILE--
<?php
$conn = new SQLite3(':memory:');
$conn->exec("CREATE TABLE foo (id INT)");

function testing($val) {
  echo $val;
	debug_print_backtrace();
	return $val;
}

$conn->createFunction('testing', 'testing', 1);

$stmt = $conn->prepare("INSERT INTO foo VALUES (testing(?))");

$stmt->bindValue(1, 1);

$res = $stmt->execute(); // Should run
$res->fetchArray(); // Should not run
$res->fetchArray(); // Should not run
$res->fetchArray(); // Should not run

$stmt->bindValue(1, 2);

$res = $stmt->execute(); // Should run
$res->fetchArray(); // Should not run
$res->fetchArray(); // Should not run
$res->fetchArray(); // Should not run

$stmt->bindValue(1, 3);

$res2 = $stmt->execute(); // Should run
$res2->fetchArray(); // Should not run
$res2->fetchArray(); // Should not run
$res2->fetchArray(); // Should not run

$res = $conn->query("SELECT * FROM foo");
while (($row = $res->fetchArray(SQLITE3_NUM))) {
    var_dump($row);
}
?>
--EXPECTF--
1#0 [internal function]: testing(1)
#1 %s(17): SQLite3Stmt->execute()
2#0 [internal function]: testing(2)
#1 %s(24): SQLite3Stmt->execute()
3#0 [internal function]: testing(3)
#1 %s(31): SQLite3Stmt->execute()
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(3)
}