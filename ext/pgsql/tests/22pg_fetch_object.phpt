--TEST--
PostgreSQL pg_fetch_*() functions
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);

include 'config.inc';

class test_class {
	function __construct($arg1, $arg2) {
		echo __METHOD__ . "($arg1,$arg2)\n";
	}
}

$db = pg_connect($conn_str);

$sql = "SELECT * FROM $table_name";
$result = pg_query($db, $sql) or die('Cannot qeury db');
$rows = pg_num_rows($result);

var_dump(pg_fetch_object($result, 'test_class', array(1, 2)));

echo "Ok\n";
?>
--EXPECT--
test_class::__construct(1,2)
object(test_class)#1 (3) {
  ["num"]=>
  string(1) "0"
  ["str"]=>
  string(3) "ABC"
  ["bin"]=>
  NULL
}
Ok
