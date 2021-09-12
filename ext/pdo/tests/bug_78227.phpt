--TEST--
Bug #78227 Prepared statements ignore RENAME COLUMN
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$db = new PDO('sqlite::memory:');
$db->exec("CREATE TABLE user (id INTEGER PRIMARY KEY NOT NULL, foo VARCHAR(255) NOT NULL)");
$db->exec("INSERT INTO user (id, foo) VALUES (10, 'test')");
$stmt = $db->prepare("SELECT * FROM user WHERE id = :id");
$stmt->execute(['id' => 10]);
print_r($stmt->fetch(PDO::FETCH_ASSOC));
$db->exec("ALTER TABLE user RENAME COLUMN foo TO bar");
$res = $stmt->execute(['id' => 10]);
print_r($stmt->fetch(PDO::FETCH_ASSOC));
?>
--EXPECT--
Array
(
    [id] => 10
    [foo] => test
)
Array
(
    [id] => 20
    [bar] => test
)
