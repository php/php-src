--TEST--
GH-23756 (crash when execute() is called from a collation callback)
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
        $stmt->execute();
    }
    return $a <=> $b;
});

$stmt = $db->prepare('SELECT x FROM t ORDER BY x COLLATE evil');
$armed = true;
try {
    $stmt->execute();
} catch (Error $e) {
    echo 'without parameters: ', $e->getMessage(), "\n";
}

$stmt = $db->prepare('SELECT x FROM t WHERE x <> :p ORDER BY x COLLATE evil');
$stmt->bindValue(':p', 'zzz');
$armed = true;
try {
    $stmt->execute();
} catch (Error $e) {
    echo 'with a bound parameter: ', $e->getMessage(), "\n";
}
?>
--EXPECT--
without parameters: Cannot execute a PDOStatement while it is executing
with a bound parameter: Cannot execute a PDOStatement while it is executing
