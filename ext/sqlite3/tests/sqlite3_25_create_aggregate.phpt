--TEST--
SQLite3::createAggregate() test
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

require_once(dirname(__FILE__) . '/new_db.inc');

function sum_list_step($context, $rows, $string) {
	if (empty($context))
	{
		$context = array('total' => 0, 'values' => array());
	}
	$context['total'] += intval($string);
	$context['values'][] = $context['total'];
	return $context;
}

function sum_list_finalize($context) {
	return implode(',', $context['values']);
}

echo "Creating Table\n";
var_dump($db->exec('CREATE TABLE test (a INTEGER, b INTEGER)'));

echo "INSERT into table\n";
var_dump($db->exec("INSERT INTO test (a, b) VALUES (1, -1)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (2, -2)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (3, -3)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (4, -4)"));
var_dump($db->exec("INSERT INTO test (a, b) VALUES (4, -4)"));

$db->createAggregate('S', 'sum_list_step', 'sum_list_finalize', 1);

print_r($db->querySingle("SELECT S(a), S(b) FROM test", true));

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECTF--
Creating Table
bool(true)
INSERT into table
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Array
(
    [S(a)] => 1,3,6,10,14
    [S(b)] => -1,-3,-6,-10,-14
)
Closing database
bool(true)
Done
