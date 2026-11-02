--TEST--
GH-24990 (Statement unusable after a failed execute())
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO('sqlite::memory:', options: [PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION]);
$db->exec('CREATE TABLE test (k TEXT PRIMARY KEY NOT NULL)');
$db->exec("INSERT INTO test VALUES ('a')");

$stmt = $db->prepare('INSERT INTO test (k) VALUES (?)');
foreach (['a', 'b', 'b', 'c'] as $k) {
    try {
        var_dump($stmt->execute([$k]));
    } catch (PDOException $e) {
        echo $e->getMessage(), "\n";
    }
}
var_dump($db->query('SELECT k FROM test ORDER BY k')->fetchAll(PDO::FETCH_COLUMN));
?>
--EXPECT--
SQLSTATE[23000]: Integrity constraint violation: 19 UNIQUE constraint failed: test.k
bool(true)
SQLSTATE[23000]: Integrity constraint violation: 19 UNIQUE constraint failed: test.k
bool(true)
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
