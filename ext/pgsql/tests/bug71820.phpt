--TEST--
Bug #71820 pg_fetch_object bind parameters before call constructor
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('config.inc');

$tableName = 'test_pg_fetch_object';

class TestRow
{

	private $set_from_constructor;
	private $data;
	private $hello = 42;

	public function __construct($set_from_constructor)
	{
		$this->set_from_constructor = $set_from_constructor;
	}

	public function __set($name, $value)
	{
		if (!isset($this->data[$name])) {
			/* $this->set_from_constructor has an expected value */
			$this->data[$name] = 42 == $this->set_from_constructor ? $value : -1;
			return;
		}
		throw new \Exception('Duplicity column name.');
	}

}

$connection = pg_connect($conn_str);

if (!$connection) {
	die('Connection faild.');
}

$table = <<<SQL
CREATE TABLE IF NOT EXISTS $tableName (
  id serial NOT NULL,
  name character varying NOT NULL
);
SQL;
pg_query($connection, $table);

pg_query_params('INSERT INTO ' . $tableName . ' (name) VALUES ($1), ($2);', ['$1' => 'Doe', '$2' => 'Joe']);

$result = pg_query('SELECT * FROM ' . $tableName . ' LIMIT 10;');

while ($row = pg_fetch_object($result, NULL, 'TestRow', [42])) {
	var_dump($row);
}

pg_query($connection, "DROP TABLE $tableName");

pg_close($connection);

?>
==DONE==
--EXPECTF--
object(TestRow)#%d (3) {
  ["set_from_constructor":"TestRow":private]=>
  int(42)
  ["data":"TestRow":private]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["name"]=>
    string(3) "Doe"
  }
  ["hello":"TestRow":private]=>
  int(42)
}
object(TestRow)#%d (3) {
  ["set_from_constructor":"TestRow":private]=>
  int(42)
  ["data":"TestRow":private]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["name"]=>
    string(3) "Joe"
  }
  ["hello":"TestRow":private]=>
  int(42)
}
==DONE==
