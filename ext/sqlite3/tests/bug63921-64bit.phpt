--TEST--
Bug #63921 sqlite3::bindvalue and relative PHP functions aren't using sqlite3_*_int64 API
--EXTENSIONS--
sqlite3
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die('skip'); // skip for 32bit builds - there is another test for that
?>
--FILE--
<?php
$num = 100004313234244; // notice this exceeds 32 bits
$conn = new sqlite3(':memory:');
$conn->query('CREATE TABLE users (id INTEGER NOT NULL, num INTEGER NOT NULL, PRIMARY KEY(id))');

$stmt = $conn->prepare('insert into users (id, num) values (:id, :num)');
$stmt->bindValue(':id', 1, SQLITE3_INTEGER);
$stmt->bindValue(':num', $num, SQLITE3_INTEGER);
$stmt->execute();

$stmt = $conn->query('SELECT num FROM users');
$result = $stmt->fetchArray();

var_dump($num,$result[0]);

?>
--EXPECT--
int(100004313234244)
int(100004313234244)
