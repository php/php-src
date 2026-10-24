--TEST--
SQLite3::close() from a destructor fired during callback cleanup
--EXTENSIONS--
sqlite3
--FILE--
<?php
/* A destructor fired while the callback's return value is released still runs
   with sqlite3 on the stack, so the close must be rejected there too. */
class Closer {
    public function __destruct() {
        global $db;
        try {
            var_dump($db->close());
        } catch (Throwable $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
}

$db = new SQLite3(':memory:');
$db->exec('CREATE TABLE t (a INTEGER)');
$db->exec('INSERT INTO t VALUES (7)');
$live = $db->prepare('SELECT a FROM t');
$res = $live->execute();

$db->createFunction('probe', function () { return new Closer(); });
$stmt = $db->prepare('SELECT probe()');
try {
    $stmt->execute();
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

var_dump($res->fetchArray(SQLITE3_NUM));
var_dump($db->close());
echo "done\n";
?>
--EXPECT--
Error: Cannot close SQLite3 database while inside a callback
Error: Object of class Closer could not be converted to string
array(1) {
  [0]=>
  int(7)
}
bool(true)
done
