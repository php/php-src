--TEST--
Bug #45798 (sqlite3 doesn't notice if variable was bound)
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

$db->exec('CREATE TABLE test (time INTEGER, id STRING)');

$db->exec("INSERT INTO test (time, id) VALUES (" . time() . ", 'a')");
$db->exec("INSERT INTO test (time, id) VALUES (" . time() . ", 'b')");

$stmt = $db->prepare("SELECT * FROM test WHERE id = ? ORDER BY id ASC");

$stmt->bindParam(1, $foo, SQLITE3_TEXT);
$results = $stmt->execute();

while ($result = $results->fetchArray(SQLITE3_NUM)) {
	var_dump($result);
}

$results->finalize();

$db->close();

print "done";

?>
--EXPECT--
done
