--TEST--
Check that exceptions from __toString() are handled correctly
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

class throws {
    function __toString() {
        throw new Exception("Sorry");
    }
}

$db = new sqlite3(':memory:');
$db->exec('CREATE TABLE t(id int, v varchar(255))');

$stmt = $db->prepare('INSERT INTO t VALUES(:i, :v)');
$stmt->bindValue('i', 1234);
$stmt->bindValue('v', new throws);

try {
    $stmt->execute();
} catch (Exception $e) {
    echo "Exception thrown ...\n";
}

try {
    $stmt->execute();
} catch (Exception $e) {
    echo "Exception thrown ...\n";
}

$query = $db->query("SELECT * FROM t");
while ($row = $query->fetchArray(SQLITE3_ASSOC)) {
    print_r($row);
}

?>
--EXPECT--
Exception thrown ...
Exception thrown ...
