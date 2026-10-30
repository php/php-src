--TEST--
GH-23756 (crash when closeCursor() is called from a collation callback)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = Pdo\Sqlite::connect('sqlite::memory:');
$db->exec('CREATE TABLE t (x TEXT)');
$db->exec("INSERT INTO t VALUES ('b'), ('a'), ('c')");

$stmt = null;
$db->createCollation('evil', function ($a, $b) use (&$stmt) {
    $stmt->closeCursor();
    return $a <=> $b;
});

$stmt = $db->prepare('SELECT x FROM t ORDER BY x COLLATE evil');
try {
    $stmt->execute();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump($db->query('SELECT x FROM t ORDER BY x')->fetchAll(PDO::FETCH_COLUMN));
?>
--EXPECT--
Cannot close the cursor of a PDOStatement while it is executing
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
