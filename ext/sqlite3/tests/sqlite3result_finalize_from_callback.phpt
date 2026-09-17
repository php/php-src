--TEST--
SQLite3Result::finalize() from within a callback must not touch the executing statement
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (7), (8)');

/* Armed only once the result object exists, so the probe runs on the
   fetchArray() step rather than the one inside execute()/query(). */
$armed = false;
$db->createFunction('probe', function ($v) use (&$armed) {
    global $res;
    if ($armed) {
        $armed = false;
        try {
            var_dump($res->finalize());
        } catch (Throwable $e) {
            echo "prepared: ", $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return $v;
});

$stmt = $db->prepare('SELECT probe(a) FROM t');
$res = $stmt->execute();
$armed = true;
var_dump($res->fetchArray(SQLITE3_NUM));
var_dump($res->finalize());

$armedQuery = false;
$db->createFunction('probeQuery', function ($v) use (&$armedQuery) {
    global $qres;
    if ($armedQuery) {
        $armedQuery = false;
        try {
            var_dump($qres->finalize());
        } catch (Throwable $e) {
            echo "query: ", $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return $v;
});

$qres = $db->query('SELECT probeQuery(a) FROM t');
$armedQuery = true;
var_dump($qres->fetchArray(SQLITE3_NUM));
var_dump($qres->finalize());
echo "done\n";
?>
--EXPECT--
prepared: Error: Cannot finalize SQLite3 result set while its statement is executing
array(1) {
  [0]=>
  int(7)
}
bool(true)
query: Error: Cannot finalize SQLite3 result set while its statement is executing
array(1) {
  [0]=>
  int(7)
}
bool(true)
done
