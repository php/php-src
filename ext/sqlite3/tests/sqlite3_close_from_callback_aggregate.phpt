--TEST--
SQLite3::close() from within a createAggregate() callback must not corrupt active statement
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (1)');
$db->exec('INSERT INTO t VALUES (2)');

$reported = false;
$db->createAggregate('agg', function ($context, $rows, $a) use (&$db, &$reported) {
    if (!$reported) {
        $reported = true;
        try {
            var_dump($db->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return (int) $context + $a;
}, function ($context, $rows) {
    return $context;
}, 1);

$stmt = $db->prepare('SELECT agg(a) FROM t');
$res = $stmt->execute();
var_dump($res->fetchArray(SQLITE3_NUM));
$res->finalize();
var_dump($db->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 database while inside a callback
array(1) {
  [0]=>
  int(3)
}
bool(true)
done
