<?php

require_once(dirname(__FILE__) . '/new_db.inc');
define('TIMENOW', time());

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (time INTEGER, id STRING)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'a')"));
var_dump($db->exec("INSERT INTO test (time, id) VALUES (" . TIMENOW . ", 'b')"));

echo "SELECTING results\n";
$results = $db->query("SELECT * FROM test ORDER BY id ASC");
while ($result = $results->fetchArray(SQLITE3_NUM))
{
	var_dump($result);
	/* Only read one row and break */
	break;
}

echo "Closing database\n";
var_dump($db->close());
echo "Check result was freed\n";
var_dump($results);
echo "Done\n";
?>
