--TEST--
SQLite3Stmt::paramCount basic test
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE foobar (id INTEGER, name STRING, city STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO foobar (id, name, city) VALUES (1, 'john', 'LA')"));
var_dump($db->exec("INSERT INTO foobar (id, name, city) VALUES (2, 'doe', 'SF')"));


$queryArray = array(
	"SELECT * FROM foobar WHERE id = ? ORDER BY id ASC",
	"SELECT * FROM foobar WHERE id = 2 ORDER BY id ASC",
	"SELECT * FROM foobar WHERE id = :id AND name = :name ORDER BY id ASC",
	"SELECT * FROM foobar WHERE id = 1 AND name = :name ORDER BY id ASC",
);

echo "SELECTING results\n";

foreach($queryArray as $key => $query) {
	$stmt = $db->prepare($query);

	echo 'Param count for query ' . ($key + 1) . ":\n";
	var_dump($stmt->paramCount());

	$result = $stmt->execute();
}

echo "Closing database\n";
$stmt = null;
$result = null;
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
SELECTING results
Param count for query 1:
int(1)
Param count for query 2:
int(0)
Param count for query 3:
int(2)
Param count for query 4:
int(1)
Closing database
bool(true)
Done
