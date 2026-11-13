--TEST--
SQLite3Stmt::close() from within a callback must not finalize the executing statement
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (7)');

$probed = false;
$db->createFunction('probe', function ($v) use (&$probed) {
    global $stmt, $other;
    if (!$probed) {
        $probed = true;
        try {
            var_dump($stmt->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
        /* An unrelated statement is not being stepped, so closing it is allowed. */
        var_dump($other->close());
    }
    return $v;
});

$other = $db->prepare('SELECT a FROM t');
$other->execute();

$stmt = $db->prepare('SELECT probe(a) FROM t');
$res = $stmt->execute();
var_dump($res->fetchArray(SQLITE3_NUM));

/* The statement survived the callback and is closable once the query is done. */
var_dump($stmt->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 statement while it is executing
bool(true)
array(1) {
  [0]=>
  int(7)
}
bool(true)
done
