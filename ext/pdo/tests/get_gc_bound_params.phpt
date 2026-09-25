--TEST--
PDOStatement::get_gc() must report bound params and columns for cycle collection
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
class Tracked {
    public $stmt;
    function __destruct() {
        echo "collected\n";
    }
}
$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE test(a INT)');
$stmt = $db->prepare('INSERT INTO test VALUES (?)');
$stmt->execute([1]);
$stmt2 = $db->prepare('SELECT a FROM test');
$stmt2->execute();
$stmt2->fetch(PDO::FETCH_ASSOC);
for ($i = 0; $i < 3; $i++) {
    $val = 1;
    $tracked = new Tracked();
    $tracked->stmt = $stmt;
    $stmt->bindParam(1, $val, PDO::PARAM_INT, 0, $tracked);
    $col = null;
    $tracked2 = new Tracked();
    $tracked2->stmt = $stmt2;
    $stmt2->bindColumn('a', $col, PDO::PARAM_STR, 0, $tracked2);
    unset($tracked, $tracked2);
}
unset($stmt, $stmt2, $db);
gc_collect_cycles();
echo "end\n";
--EXPECT--
collected
collected
collected
collected
collected
collected
end
