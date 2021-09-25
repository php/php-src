--TEST--
Bug #63916 PDO::PARAM_INT casts to 32bit int internally even on 64bit builds in pdo_sqlite
--EXTENSIONS--
pdo_sqlite
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die('skip');
?>
--FILE--
<?php
$num = 100004313234244; // exceeds 32 bits
$conn = new PDO('sqlite::memory:');
$conn->query('CREATE TABLE users (id INTEGER NOT NULL, num INTEGER NOT NULL, PRIMARY KEY(id))');

$stmt = $conn->prepare('insert into users (id, num) values (:id, :num)');
$stmt->bindValue(':id', 1, PDO::PARAM_INT);
$stmt->bindValue(':num', $num, PDO::PARAM_INT);
$stmt->execute();

$stmt = $conn->query('SELECT num FROM users');
$result = $stmt->fetchAll(PDO::FETCH_COLUMN);

var_dump($num,$result[0]);

?>
--EXPECT--
int(100004313234244)
int(100004313234244)
