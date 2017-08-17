--TEST--
Bug #42378 (bind_result memory exhaustion, SELECT column, FORMAT(...) AS _format)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
memory_limit=83886080
--FILE--
<?php
	require_once("connect.inc");

	function create_table($link, $column, $min, $max, $engine, $offset) {

		if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
			printf("[%03d] Cannot drop table test, [%d] %s\n",
				$offset,
				mysqli_errno($link), mysqli_error($link));
			return array();
		}
		print "$column\n";

		$sql = sprintf("CREATE TABLE test(id INT AUTO_INCREMENT PRIMARY KEY, col1 %s) ENGINE=%s",
			$column, $engine);
		if (!mysqli_query($link, $sql)) {
			printf("[%03d] Cannot create table test, [%d] %s\n",
				$offset + 1,
				mysqli_errno($link), mysqli_error($link));
			return array();
		}

		$values = array();
		for ($i = 1; $i <= 100; $i++) {
			$col1 = mt_rand($min, $max);
			$values[$i] = $col1;
			$sql = sprintf("INSERT INTO test(id, col1) VALUES (%d, %f)",
				$i, $col1);
			if (!mysqli_query($link, $sql)) {
				printf("[%03d] Cannot insert data, [%d] %s\n",
					$offset + 2,
					mysqli_errno($link), mysqli_error($link));
				return array();
			}
		}

		return $values;
	}

	function test_format($link, $format, $from, $order_by, $expected, $offset) {

		if (!$stmt = mysqli_stmt_init($link)) {
			printf("[%03d] Cannot create PS, [%d] %s\n",
				$offset,
				mysqli_errno($link), mysqli_error($link));
			return false;
		}
		print "$format\n";

		if ($order_by)
			$sql = sprintf('SELECT %s AS _format FROM %s ORDER BY %s', $format, $from, $order_by);
		else
			$sql = sprintf('SELECT %s AS _format FROM %s', $format, $from);

		if (!mysqli_stmt_prepare($stmt, $sql)) {
			printf("[%03d] Cannot prepare PS, [%d] %s\n",
				$offset + 1,
				mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_execute($stmt)) {
			printf("[%03d] Cannot execute PS, [%d] %s\n",
				$offset + 2,
				mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!mysqli_stmt_store_result($stmt)) {
			printf("[%03d] Cannot store result set, [%d] %s\n",
				$offset + 3,
				mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			return false;
		}

		if (!is_array($expected)) {

			$result = null;
			if (!mysqli_stmt_bind_result($stmt, $result)) {
				printf("[%03d] Cannot bind result, [%d] %s\n",
					$offset + 4,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}

			if (!mysqli_stmt_fetch($stmt)) {
				printf("[%03d] Cannot fetch result,, [%d] %s\n",
					$offset + 5,
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
				return false;
			}

			if ($result !== $expected) {
				printf("[%03d] Expecting %s/%s got %s/%s with %s - %s.\n",
					$offset + 6,
					gettype($expected), $expected,
					gettype($result), $result,
					$format, $sql);
			}

		} else {

			$order_by_col = $result = null;
			if (!is_null($order_by)) {
				if (!mysqli_stmt_bind_result($stmt, $order_by_col, $result)) {
					printf("[%03d] Cannot bind result, [%d] %s\n",
						$offset + 7,
						mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
					return false;
				}
			} else {
				if (!mysqli_stmt_bind_result($stmt, $result)) {
					printf("[%03d] Cannot bind result, [%d] %s\n",
						$offset + 7,
						mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
					return false;
				}
			}

			foreach ($expected as $k => $v) {
				if (!mysqli_stmt_fetch($stmt)) {
					break;
				}
				if ($result !== $v) {
					printf("[%03d] Row %d - expecting %s/%s got %s/%s [%s] with %s - %s.\n",
						$offset + 8,
						$k,
						gettype($v), $v,
						gettype($result), $result,
						$order_by_col,
						$format, $sql);
				}
			}

		}

		mysqli_stmt_free_result($stmt);
		mysqli_stmt_close($stmt);

		return true;
	}

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect - [%d] %s\n",
			mysqli_connect_errno(),
			mysqli_connect_error());

	/* create new table and select from it */
	$expected = create_table($link, 'FLOAT', -10000, 10000, $engine, 90);
	foreach ($expected as $k => $v)
		$expected[$k] = number_format(round($v), 0, '.', ',');
	test_format($link, 'FORMAT(col1, 0)', 'test', NULL, array(), 100);

	$expected = create_table($link, 'FLOAT', -10000, 10000, $engine, 110);
	foreach ($expected as $k => $v)
		$expected[$k] = number_format(round($v), 0, '.', ',');
	test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 120);

	$expected = create_table($link, 'FLOAT UNSIGNED', 0, 10000, $engine, 130);
	foreach ($expected as $k => $v)
		$expected[$k] = number_format(round($v), 0, '.', ',');
	test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 140);

	$expected = create_table($link, 'DECIMAL(5,0)', -1000, 1000, $engine, 150);
	foreach ($expected as $k => $v)
		$expected[$k] = number_format(round($v), 0, '.', ',');
	test_format($link, 'id AS order_by_col, FORMAT(col1, 0)', 'test', 'id', $expected, 160);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
FLOAT
FORMAT(col1, 0)
FLOAT
id AS order_by_col, FORMAT(col1, 0)
FLOAT UNSIGNED
id AS order_by_col, FORMAT(col1, 0)
DECIMAL(5,0)
id AS order_by_col, FORMAT(col1, 0)
done!
