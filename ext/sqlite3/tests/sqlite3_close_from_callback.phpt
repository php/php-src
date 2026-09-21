--TEST--
SQLite3::close() from within a UDF callback must not corrupt active statement
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');
$db->createFunction('boom', function () use ($db) {
    try {
        var_dump($db->close());
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    return 1;
});
$stmt = $db->prepare('SELECT boom()');
var_dump($stmt !== false);
$res = $stmt->execute();
var_dump($res !== false);
var_dump($res->fetchArray(SQLITE3_NUM));
$res->finalize();
var_dump($db->close());
echo "done\n";
?>
--EXPECT--
bool(true)
Error: Cannot close SQLite3 database while inside a callback
bool(true)
Error: Cannot close SQLite3 database while inside a callback
array(1) {
  [0]=>
  int(1)
}
bool(true)
done
