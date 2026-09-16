--TEST--
SQLite3::close() from within a setAuthorizer() callback must not finalize live statements
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (7)');
$live = $db->prepare('SELECT a FROM t');

$reported = false;
$db->setAuthorizer(function ($action, $arg1, $arg2, $arg3, $arg4) use (&$db, &$reported) {
    if (!$reported) {
        $reported = true;
        try {
            var_dump($db->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return SQLite3::OK;
});

$db->prepare('SELECT 1');

$res = $live->execute();
var_dump($res->fetchArray(SQLITE3_NUM));
$res->finalize();
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 database while inside a callback
array(1) {
  [0]=>
  int(7)
}
done
