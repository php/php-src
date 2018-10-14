--TEST--
mysqli_stmt_get_result() - SHOW, DESCRIBE, EXPLAIN
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('mysqli_stmt_get_result'))
	die('skip mysqli_stmt_get_result not available');
?>
--FILE--
<?php
	/*
	NOTE: no datatype tests here! This is done by
	mysqli_stmt_bind_result.phpt already. Restrict
	this test case to the basics.
	*/
	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (mysqli_query($link, 'PREPARE mystmt FROM "SHOW ENGINES"')) {
		mysqli_query($link, 'DEALLOCATE PREPARE mystmt');

		if (!$stmt->prepare('SHOW ENGINES') ||
			!$stmt->execute())
			printf("[002] [%d] %s\n", $stmt->errno, $stmt->error);

		if (!$res = $stmt->get_result())
			printf("[003] [%d] %s\n", $stmt->errno, $stmt->error);

		$engines = mysqli_fetch_all($res, MYSQLI_NUM);
		if	(empty($engines)) {
			printf("[004] It is very unlikely that SHOW ENGINES returns no data, check manually\n");
		} else {
			$found = false;
			foreach ($engines as $k => $engine)
				foreach ($engine as $k => $v)
					if (stristr('MyISAM', $v)) {
						$found = true;
						break;
					}
			if (!$found)
				printf("[005] It is very unlikely that SHOW ENGINES does not show MyISAM, check manually\n");
		}
		mysqli_free_result($res);
	}

	if (mysqli_query($link, 'PREPARE mystmt FROM "DESCRIBE test id"')) {
		mysqli_query($link, 'DEALLOCATE PREPARE mystmt');

		if (!$stmt->prepare('DESCRIBE test id') ||
			!$stmt->execute())
			printf("[006] [%d] %s\n", $stmt->errno, $stmt->error);

		if (!$res = $stmt->get_result())
			printf("[007] [%d] %s\n", $stmt->errno, $stmt->error);

		$description = mysqli_fetch_assoc($res);
		if ($description['Field'] != 'id') {
			printf("[008] Returned data seems wrong, [%d] %s\n",
				mysqli_errno($link), mysqli_error($link));
			var_dump($description);
		}
		mysqli_free_result($res);
	}

	if (mysqli_query($link, 'PREPARE mystmt FROM "EXPLAIN SELECT id FROM test"')) {
		mysqli_query($link, 'DEALLOCATE PREPARE mystmt');

		if (!$stmt->prepare('EXPLAIN SELECT id FROM test') ||
			!$stmt->execute())
			printf("[009] [%d] %s\n", $stmt->errno, $stmt->error);

		if (!$res = $stmt->get_result())
			printf("[010] [%d] %s\n", $stmt->errno, $stmt->error);

		$tmp = mysqli_fetch_assoc($res);
		if (empty($tmp))
			printf("[011] Empty EXPLAIN result set seems wrong, check manually, [%d] %s\n",
				mysqli_errno($link), mysqli_error($link));
		mysqli_free_result($res);
	}
	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
done!
