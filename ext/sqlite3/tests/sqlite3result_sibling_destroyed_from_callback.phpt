--TEST--
Dropping a sibling SQLite3Result from a callback must not reset the executing statement
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (1),(2),(3),(4),(5)');

/* Two result objects wrap one statement; the idle one is reachable from
   userland and its destructor would otherwise reset the live statement. */
$n = 0;
$db->createFunction('probe', function ($v) use (&$n) {
    global $idle;
    if ($n++ == 2) {
        $idle = null;
        echo "sibling dropped\n";
    }
    return $v;
});

$stmt = $db->prepare('SELECT probe(a) FROM t');
$idle = $stmt->execute();
$live = $stmt->execute();
$rows = [];
while (($r = $live->fetchArray(SQLITE3_NUM)) !== false) {
    $rows[] = $r[0];
}
echo implode(',', $rows), "\n";
echo "done\n";
?>
--EXPECT--
sibling dropped
1,2,3,4,5
done
