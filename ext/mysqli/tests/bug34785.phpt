--TEST--
Bug #34785 (Can not properly subclass mysqli_stmt)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
	include ("connect.inc");

	class my_stmt extends mysqli_stmt
	{
		public function __construct($link, $query) {
			parent::__construct($link, $query);
		}
	}

	class my_result extends mysqli_result
	{
		public function __construct($link, $query) {
			parent::__construct($link, $query);
		}
	}

	/*** test mysqli_connect 127.0.0.1 ***/
	$link = mysqli_connect($host, $user, $passwd);
	mysqli_query($link, "SET sql_mode=''");

	$stmt = new my_stmt($link, "SELECT 'foo' FROM DUAL");

	$stmt->execute();
	$stmt->bind_result($var);
	$stmt->fetch();

	$stmt->close();
	var_dump($var);

	mysqli_real_query($link, "SELECT 'bar' FROM DUAL");
	$result = new my_result($link, MYSQLI_STORE_RESULT);
	$row = $result->fetch_row();
	$result->close();

	var_dump($row[0]);

	mysqli_close($link);
?>
--EXPECT--
string(3) "foo"
string(3) "bar"
