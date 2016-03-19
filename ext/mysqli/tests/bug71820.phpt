--TEST--
Bug #71820 __set has to be called after constructor, mysqli part
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");
?>
--FILE--
<?php

include "connect.inc";

$tableName = 'test_mysqli_fetch_object';

class TestRow
{

	private $set_from_constructor;
	private $data;
	private $hello = "world";

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


if (!($connection = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))) {
	printf("[001] Cannot connect to the server");
}
	
$rc = mysqli_query($connection, "DROP TABLE IF EXISTS $tableName");
if (!$rc)
	printf("[002] [%d] %s\n", mysqli_errno($connection), mysqli_error($connection));

$table = <<<SQL
CREATE TABLE $tableName (
  id int NOT NULL auto_increment primary key,
  name varchar(255) NOT NULL
);
SQL;

$rc = mysqli_query($connection, $table);
if (!$rc)
	printf("[003] [%d] %s\n", mysqli_errno($connection), mysqli_error($connection));

$rc = mysqli_query($connection, "INSERT INTO " . $tableName . " (name) VALUES ('Doe'), ('Joe')");
if (!$rc)
	printf("[004] [%d] %s\n", mysqli_errno($connection), mysqli_error($connection));

$result = mysqli_query($connection, 'SELECT * FROM ' . $tableName . ' LIMIT 10');
if (!$result)
	printf("[005] [%d] %s\n", mysqli_errno($result), mysqli_error($result));


while ($row = mysqli_fetch_object($result, 'TestRow', [42])) {
	var_dump($row);
}

mysqli_close($connection);

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
  string(5) "world"
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
  string(5) "world"
}
==DONE==
