--TEST--
SQLite3Stmt::close() from within a collation callback
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE s (v TEXT)');
$db->exec("INSERT INTO s VALUES ('b'), ('a')");

$probed = false;
$db->createCollation('coll', function ($a, $b) use (&$probed) {
    global $collStmt;
    if (!$probed) {
        $probed = true;
        try {
            var_dump($collStmt->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return strcmp($a, $b);
});

$collStmt = $db->prepare('SELECT v FROM s ORDER BY v COLLATE coll');
$res = $collStmt->execute();
var_dump($res->fetchArray(SQLITE3_NUM));
var_dump($collStmt->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 statement while it is executing
array(1) {
  [0]=>
  string(1) "a"
}
bool(true)
done
