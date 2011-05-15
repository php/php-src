--TEST--
Bind limits
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	function bind_many($offset, $link, $num_params, $rows, $eval = true) {

		$drop = "DROP TABLE IF EXISTS test";
		$create = "CREATE TABLE test(id INT AUTO_INCREMENT PRIMARY KEY, ";
		$insert = "INSERT INTO test";
		$columns = "";
		$values = "";
		$stmt_params = "";
		$params = array();
		for ($i = 0; $i < $num_params; $i++) {
			$create 		.= "col" . $i . " INT, ";
			$columns 		.= "col" . $i . ", ";
			$values 		.= "?, ";
			$stmt_params 	.= '$params[' . $i . '], ';
			for ($j = 0; $j < $rows; $j++)
			  $params[($j * $rows) + $i] = $i;
		}
		$create = substr($create, 0, -2) . ")";

		$stmt_types = str_repeat("i", $num_params * $rows);
		$stmt_params = substr(str_repeat($stmt_params, $rows), 0, -2);
		$values = substr($values, 0, -2);
		$insert .= "(" . substr($columns, 0, -2) . ") VALUES ";
		$insert .= substr(str_repeat("(" . $values . "), ", $rows), 0, -2);

		$stmt_bind_param = 'return mysqli_stmt_bind_param($stmt, "' . $stmt_types . '", ' . $stmt_params . ');';

		printf("Testing %d columns with %d rows...\n", $num_params, $rows);

		if (!$link->query($drop) || !$link->query($create)) {
			printf("[%03d + 01] [%d] %s\n", $offset, $link->errno, $link->error);
			return false;
		}
		printf("... table created\n");

		if (!$stmt = $link->prepare($insert)) {
			printf("[%03d + 02] [%d] %s\n", $offset, $link->errno, $link->error);
			return false;
		}
		if ($stmt->param_count != $num_params * $rows) {
			  printf("[%03d + 03] Parameter count should be %d but got %d\n", $offset, $num_params * $rows, $stmt->param_count);
			return false;
		}
		printf("... statement with %d parameters prepared\n", $stmt->param_count);

		if ($eval) {
			if (!eval($stmt_bind_param)) {
				printf("[%03d + 03] [%d] %s\n", $offset, $stmt->errno, $stmt->error);
				return false;
			}
		} else {
			$param_ref = array($stmt_types);
			for ($i = 0; $i < $rows; $i++)
				for ($j = 0; $j < $num_params; $j++)
					$param_ref[] = &$params[($i * $rows) + $j];

			if (!call_user_func_array(array($stmt, 'bind_param'), $param_ref)) {
				printf("[%03d + 03] [%d] %s\n", $offset, $stmt->errno, $stmt->error);
				return false;
			}
		}
		if ($stmt->param_count != $num_params * $rows) {
			 printf("[%03d + 03] Parameter count should be %d but got %d\n", $offset, $num_params * $rows, $stmt->param_count);
			return false;
		}

		if (!$stmt->execute()) {
			printf("[%03d + 04] [%d] %s\n", $offset, $stmt->errno, $stmt->error);
			return false;
		}
		printf("Statement done\n");

		$stmt->close();

		if (!($res = $link->query("SELECT * FROM test"))) {
			printf("[%03d + 05] [%d] %s\n", $offset, $link->errno, $link->error);
			return false;
		}

		$row = $res->fetch_row();
		$res->close();

		for ($i = 0; $i < $num_params; $i++) {
			if ($row[$i + 1] != $i) {
				printf("[%03d + 06] [%d] %s\n", $offset, $link->errno, $link->error);
			}
		}

		return true;
	}

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}

	var_dump(bind_many(10, $link, 273, 240, true));
	var_dump(bind_many(20, $link, 273, 240, false));
	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Testing 273 columns with 240 rows...
... table created
... statement with 65520 parameters prepared
Statement done
bool(true)
Testing 273 columns with 240 rows...
... table created
... statement with 65520 parameters prepared
Statement done
bool(true)
done!