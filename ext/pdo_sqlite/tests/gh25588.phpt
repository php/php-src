--TEST--
GH-25588 (Use-after-free of a bound string after its PDO parameter is released)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE t (a TEXT, b TEXT)');
$db->exec("INSERT INTO t VALUES ('xxxxxxxxxx', NULL), ('xxxxxxxxxx', NULL), ('xxxxxxxxxx', NULL)");

$stmt = $db->prepare('INSERT INTO t VALUES (:a, :b)');
$stmt->execute([':a' => str_repeat('a', 10), ':b' => str_repeat('b', 10)]);
$stmt->execute([':a' => 1234567890]);
echo "execute() with fewer params:\n";
var_dump($db->query('SELECT a, b FROM t WHERE b IS NOT NULL ORDER BY rowid')->fetchAll(PDO::FETCH_NUM));

$stmt = $db->prepare('SELECT rowid FROM t WHERE a = :a ORDER BY rowid');
$stmt->bindValue(':a', str_repeat('x', 10));
$stmt->execute();
echo "bindValue() during fetch:\n";
var_dump($stmt->fetchColumn());
$stmt->bindValue(':a', str_repeat('q', 10));
$reuse = str_repeat('w', 10);
while (($id = $stmt->fetchColumn()) !== false) {
    var_dump($id);
}

$value = str_repeat('x', 10);
$stmt = $db->prepare('SELECT rowid FROM t WHERE a = :a ORDER BY rowid');
$stmt->bindParam(':a', $value);
$stmt->execute();
echo "bindParam() variable reassigned during fetch:\n";
var_dump($stmt->fetchColumn());
$value = str_repeat('q', 10);
$reuse = str_repeat('w', 10);
while (($id = $stmt->fetchColumn()) !== false) {
    var_dump($id);
}

$stmt = $db->prepare('INSERT INTO t VALUES (:a, :b)');
$stmt->bindValue(':a', str_repeat('c', 10));
$stmt->bindValue(':b', str_repeat('d', 10), PDO::PARAM_LOB);
$stmt->execute();
$stmt->execute([':a' => 1234567891]);
echo "PARAM_LOB omitted on execute():\n";
var_dump($db->query("SELECT a, b FROM t WHERE a IN ('cccccccccc', '1234567891') ORDER BY rowid")->fetchAll(PDO::FETCH_NUM));

echo "PARAM_LOB bind failure:\n";
$stmt = $db->prepare('SELECT :a');
$stmt->bindValue(':nope', 'x', PDO::PARAM_LOB);
try {
    $stmt->execute();
} catch (PDOException $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
execute() with fewer params:
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(10) "aaaaaaaaaa"
    [1]=>
    string(10) "bbbbbbbbbb"
  }
  [1]=>
  array(2) {
    [0]=>
    string(10) "1234567890"
    [1]=>
    string(10) "bbbbbbbbbb"
  }
}
bindValue() during fetch:
int(1)
int(2)
int(3)
bindParam() variable reassigned during fetch:
int(1)
int(2)
int(3)
PARAM_LOB omitted on execute():
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(10) "cccccccccc"
    [1]=>
    string(10) "dddddddddd"
  }
  [1]=>
  array(2) {
    [0]=>
    string(10) "1234567891"
    [1]=>
    string(10) "dddddddddd"
  }
}
PARAM_LOB bind failure:
PDOException: SQLSTATE[HY000]: General error: 25 column index out of range
