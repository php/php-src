--TEST--
SQLite3Stmt::close() from within an aggregate callback
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (1), (2)');

$probed = false;
$db->createAggregate('agg', function ($ctx, $rows, $v) use (&$probed) {
    global $aggStmt;
    if (!$probed) {
        $probed = true;
        try {
            var_dump($aggStmt->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return $ctx + $v;
}, function ($ctx, $rows) { return $ctx; });

$aggStmt = $db->prepare('SELECT agg(a) FROM t');
$res = $aggStmt->execute();
var_dump($res->fetchArray(SQLITE3_NUM));
var_dump($aggStmt->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 statement while it is executing
array(1) {
  [0]=>
  int(3)
}
bool(true)
done
