--TEST--
Bug #64531 (SQLite3Result::fetchArray runs the query again)
--SKIPIF--
<?php
if (!extension_loaded('sqlite3')) die('skip sqlite3 extension not available');
?>
--FILE--
<?php
function testing($val) {
	echo "Testing with: $val\n";
	return true;
}

$conn = new SQLite3(':memory:');
$conn->exec("CREATE TABLE foo (id INT)");
for ($i = 1; $i <= 3; $i++) {
    $conn->exec("INSERT INTO foo VALUES ($i)");
}
$conn->createFunction('testing', 'testing', 1);

$res = $conn->query('SELECT * FROM foo WHERE testing(id)');
$arr = $res->fetchArray();
$arr = $res->fetchArray();
$arr = $res->fetchArray();
$arr = $res->fetchArray();
?>
--EXPECT--
Testing with: 1
Testing with: 2
Testing with: 3
