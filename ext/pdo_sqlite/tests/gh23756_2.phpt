--TEST--
GH-23756 (crash when closeCursor() is called from a collation callback while fetching)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = Pdo\Sqlite::connect('sqlite::memory:');
$db->exec('CREATE TABLE t (x TEXT)');
$db->exec("INSERT INTO t VALUES ('b'), ('a'), ('c')");

$stmt = null;
$armed = false;
$db->createCollation('evil', function ($a, $b) use (&$stmt, &$armed) {
    if ($armed) {
        $armed = false;
        $stmt->closeCursor();
    }
    return $a <=> $b;
});

$stmt = $db->prepare("SELECT x FROM t WHERE x <> 'zzz' COLLATE evil");
$stmt->execute();
var_dump($stmt->fetchColumn());

$armed = true;
try {
    $stmt->fetchColumn();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(1) "b"
Cannot close the cursor of a PDOStatement while it is executing
