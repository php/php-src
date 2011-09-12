--TEST--
EXPLAIN - metadata
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");
if (!$IS_MYSQLND)
  die("skip Open libmysql/MySQL issue http://bugs.mysql.com/?id=62350");
?>
--FILE--
<?php
	require_once('connect.inc');
	require_once('table.inc');

	if (!$res = mysqli_query($link, 'EXPLAIN SELECT t1.*, t2.* FROM test AS t1, test AS t2'))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$num_rows 	= 0;
	$num_fields 	= 0;
	$field_names	= array();
	if (!$row = mysqli_fetch_assoc($res)) {
		printf("[002] Expecting result but got no data [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));
	} else {
		$num_rows++;
		$num_fields = count($row);
		foreach ($row as $name => $value)
			$field_names[$name] = gettype($value);
	}

	while ($row = mysqli_fetch_assoc($res))
		$num_rows++;

	if (($tmp = mysqli_num_rows($res)) !== $num_rows) {
		printf("[003] Expecting int/%d got %s/%s\n",
			$num_rows, gettype($tmp), $tmp);
	}
	if (($tmp = mysqli_field_count($link)) !== $num_fields) {
		printf("[004] Expecting int/%d got %s/%s\n",
			$num_fields, gettype($tmp), $tmp);
	}
	$fields = mysqli_fetch_fields($res);
	if (($tmp = count($fields)) !== $num_fields) {
		printf("[005] Expecting int/%d got %s/%s\n",
			$num_fields, gettype($tmp), $tmp);
	}

	foreach ($fields as $k => $field) {
		$field->max_length = 0;// change it or we will get diff error
		if (isset($field_names[$field->name])) {
			unset($field_names[$field->name]);
		} else {
			printf("[006] Unexpected field '%s', dumping info\n");
			var_dump($field);
		}
	}
	if (!empty($field_names)) {
		printf("[007] Field descriptions missing for the following columns\n");
		var_dump($field_names);
	}

	mysqli_free_result($res);

	$stmt = mysqli_stmt_init($link);
	/* Depending on your version, the MySQL server migit not support this */
	if ($stmt->prepare('EXPLAIN SELECT t1.*, t2.* FROM test AS t1, test AS t2') && $stmt->execute()) {
		if (!mysqli_stmt_store_result($stmt))
			printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (!$res_meta = mysqli_stmt_result_metadata($stmt))
			printf("[009] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

		if (($tmp = mysqli_stmt_num_rows($stmt)) !== $num_rows) {
			printf("[010] Expecting int/%d got %s/%s\n",
				$num_rows, gettype($tmp), $tmp);
		}
		if (($tmp = mysqli_stmt_field_count($stmt)) !== $num_fields) {
			printf("[011] Expecting int/%d got %s/%s\n",
				$num_fields, gettype($tmp), $tmp);
		}
		if (($tmp = mysqli_field_count($link)) !== $num_fields) {
			printf("[013] Expecting int/%d got %s/%s\n",
				$num_fields, gettype($tmp), $tmp);
		}
		if (($tmp = $res_meta->field_count) !== $num_fields) {
			printf("[014] Expecting int/%d got %s/%s\n",
				$num_fields, gettype($tmp), $tmp);
		}
		$fields_res_meta = mysqli_fetch_fields($res_meta);
		if (($tmp = count($fields_res_meta)) !== $num_fields)
			printf("[015] Expecting int/%d got %s/%s\n",
				$num_fields, gettype($tmp), $tmp);

		if ($fields_res_meta != $fields) {
			printf("[016] Prepared Statement metadata differs from normal metadata, dumping\n");
			var_dump($fields_res_meta);
			var_dump($fields);
		}

		if (function_exists('mysqli_stmt_get_result') &&
			$stmt->prepare('EXPLAIN SELECT t1.*, t2.* FROM test AS t1, test AS t2') &&
			$stmt->execute()) {
			if (!$res_stmt = mysqli_stmt_get_result($stmt)) {
				printf("[017] Cannot fetch result from PS [%d] %s\n",
					mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
			}
			if (($tmp = mysqli_num_rows($res_stmt)) !== $num_rows) {
				printf("[018] Expecting int/%d got %s/%s\n",
					$num_rows, gettype($tmp), $tmp);
			}
			if ((mysqli_stmt_num_rows($stmt)) !== 0) {
				printf("[019] Expecting int/0 got %s/%s\n", gettype($tmp), $tmp);
			}
			if (($tmp = mysqli_stmt_field_count($stmt)) !== $num_fields) {
				printf("[020] Expecting int/%d got %s/%s\n",
					$num_fields, gettype($tmp), $tmp);

			}
			if (($tmp = $res_stmt->field_count) !== $num_fields) {
				printf("[021] Expecting int/%d got %s/%s\n",
					$num_fields, gettype($tmp), $tmp);
			}

			$fields_stmt = mysqli_fetch_fields($res_stmt);
			if (($tmp = count($fields_stmt)) !== $num_fields) {
				printf("[022] Expecting int/%d got %s/%s\n",
					$num_fields, gettype($tmp), $tmp);
			}
			reset($fields);
			foreach ($fields_stmt as $fields_stmt_val) {
				list(,$fields_val) = each($fields);
				unset($fields_stmt_val->max_length);
				unset($fields_val->max_length);
				if ($fields_stmt_val != $fields_val) {
					printf("[023] PS mysqli_stmt_get_result() metadata seems wrong, dumping\n");
					var_dump($fields_stmt_val);
					var_dump($fields_val);
				}
			}
/*
			if ($fields_stmt != $fields) {
				printf("[023] PS mysqli_stmt_get_result() metadata seems wrong, dumping\n");
				var_dump($fields_stmt);
				var_dump($fields);
			}
*/
			mysqli_free_result($res_stmt);
		}
	}
	mysqli_stmt_close($stmt);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!
