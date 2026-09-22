--TEST--
SQLite3::close() from within a createCollation() callback must not corrupt active statement
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (s TEXT)');
$db->exec("INSERT INTO t VALUES ('b')");
$db->exec("INSERT INTO t VALUES ('a')");

$reported = false;
$db->createCollation('CB', function ($x, $y) use (&$db, &$reported) {
    if (!$reported) {
        $reported = true;
        try {
            var_dump($db->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return strcmp($x, $y);
});

$stmt = $db->prepare('SELECT s FROM t ORDER BY s COLLATE CB');
$res = $stmt->execute();
while ($row = $res->fetchArray(SQLITE3_NUM)) {
    echo $row[0], "\n";
}
$res->finalize();
var_dump($db->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 database while inside a callback
a
b
bool(true)
done
