--TEST--
__toString() exception during PDO Sqlite parameter binding
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class throws {
    function __toString() {
        throw new Exception("Sorry");
    }
}

$db = new PDO('sqlite::memory:');
$db->exec('CREATE TABLE t(id int, v varchar(255))');

$stmt = $db->prepare('INSERT INTO t VALUES(:i, :v)');
$param1 = 1234;
$stmt->bindValue('i', $param1);
$param2 = "foo";
$stmt->bindParam('v', $param2);

$param2 = new throws;

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
while ($row = $query->fetch(PDO::FETCH_ASSOC)) {
    print_r($row);
}

?>
--EXPECT--
Exception thrown ...
Exception thrown ...
