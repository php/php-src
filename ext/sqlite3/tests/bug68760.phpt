--TEST--
Bug #68760 (Callback throws exception behaviour. Segfault in 5.6)
--EXTENSIONS--
sqlite3
--FILE--
<?php
function oopsFunction($a, $b) {
    echo "callback".PHP_EOL;
    throw new \Exception("oops");
}

$db = new SQLite3(":memory:");
$db->exec("CREATE TABLE test (col1 string)");
$db->exec("INSERT INTO test VALUES ('a1')");
$db->exec("INSERT INTO test VALUES ('a10')");
$db->exec("INSERT INTO test VALUES ('a2')");

try {
    $db->createCollation('NATURAL_CMP', 'oopsFunction');
    $naturalSort = $db->query("SELECT col1 FROM test ORDER BY col1 COLLATE NATURAL_CMP");
    while ($row = $naturalSort->fetchArray()) {
        echo $row['col1'], "\n";
    }
    $db->close();
}
catch(\Exception $e) {
    echo "Exception: ".$e->getMessage();
}
?>
--EXPECT--
callback
Exception: oops
