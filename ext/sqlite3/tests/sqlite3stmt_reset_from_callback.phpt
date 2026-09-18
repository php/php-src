--TEST--
SQLite3Stmt::reset() and SQLite3Result::reset() from within a callback
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (1),(2),(3),(4),(5)');

/* Fires on a later row, so a subsequent step would walk the reset statement. */
$n = 0;
$db->createFunction('probe', function ($v) use (&$n) {
    global $stmt, $res;
    if ($n++ == 1) {
        try {
            var_dump($stmt->reset());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
        try {
            var_dump($res->reset());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return $v;
});

$stmt = $db->prepare('SELECT probe(a) FROM t');
$res = $stmt->execute();
$rows = [];
while (($r = $res->fetchArray(SQLITE3_NUM)) !== false) {
    $rows[] = $r[0];
}
echo implode(',', $rows), "\n";
var_dump($stmt->reset());
var_dump($stmt->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot reset SQLite3 statement while it is executing
Error: Cannot reset SQLite3 result set while its statement is executing
1,2,3,4,5
bool(true)
bool(true)
done
