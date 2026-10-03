--TEST--
GH-23756 (closeCursor() still works after a collation callback bails out)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = Pdo\Sqlite::connect('sqlite::memory:');
$db->exec('CREATE TABLE t (x TEXT)');
$db->exec("INSERT INTO t VALUES ('b'), ('a'), ('c')");

$db->createCollation('evil', function ($a, $b) {
    exit("bailing out\n");
});

register_shutdown_function(function () use (&$stmt) {
    var_dump($stmt->closeCursor());
});

$stmt = $db->prepare('SELECT x FROM t ORDER BY x COLLATE evil');
$stmt->execute();
?>
--EXPECT--
bailing out
bool(true)
