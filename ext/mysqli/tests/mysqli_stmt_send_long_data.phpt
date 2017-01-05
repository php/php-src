--TEST--
mysqli_stmt_send_long_data()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_stmt_send_long_data()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_stmt_send_long_data($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	if (!$stmt = mysqli_stmt_init($link))
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (NULL !== ($tmp = @mysqli_stmt_send_long_data($stmt, '')))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
		printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, sprintf("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT, label LONGBLOB, PRIMARY KEY(id)) ENGINE = %s", $engine)))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, "INSERT INTO test(id, label) VALUES (?, ?)"))
		printf("[007] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = null;
	$label = null;
	if (!mysqli_stmt_bind_param($stmt, "ib", $id, $label))
		printf("[008] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!$res = mysqli_query($link, "SHOW VARIABLES LIKE 'max_allowed_packet'"))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if (isset($row['VARIABLE_VALUE']) && !isset($row['Value']))
		// MySQL 6.0
		$row['Value'] = $row['VARIABLE_VALUE'];

	if (0 === ($max_allowed_packet = (int)$row['Value']))
		printf("[011] Cannot determine max_allowed_packet size and/or bogus max_allowed_packet setting used.\n");

	// let's ignore upper limits for LONGBLOB (2^32) ...
	// maximum packet size up to which we test is 10M
	$tmp = '';
	$blob = '';
	for ($i = 0; $i < 1024; $i++) {
		$tmp .= 'a';
	}

	$limit = min(floor($max_allowed_packet / 1024 / 2), 10240);
	for ($i = 0; $i < $limit; $i++)
			$blob .= $tmp;
	/*
	if (floor($max_allowed_packet / 1024) <= 10240) {
			$limit = strlen($blob) - $max_allowed_packet - 1;
			for ($i = 0; $i < $limit; $i++)
					$blob .= 'a';
	}
	*/
	assert(strlen($blob) <= $max_allowed_packet);

	if (false !== ($tmp = mysqli_stmt_send_long_data($stmt, -1, $blob)))
		printf("[012] Expecting boolean/false, got %s/%s. [%d] %s\n",
			gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (NULL !== ($tmp = @mysqli_stmt_send_long_data($stmt, PHP_INT_MAX + 1, $blob)))
		printf("[013] Expecting NULL, got %s/%s. [%d] %s\n",
			gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (false !== ($tmp = mysqli_stmt_send_long_data($stmt, 999, $blob)))
		printf("[014] Expecting boolean/false, got %s/%s. [%d] %s\n",
			gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (true !== ($tmp = mysqli_stmt_send_long_data($stmt, 1, $blob)))
		printf("[015] Expecting boolean/true, got %s/%s. [%d] %s\n",
			gettype($tmp), $tmp, mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	$id = 1;
	if (true !== mysqli_stmt_execute($stmt))
		printf("[016] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));
	mysqli_stmt_close($stmt);

	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id"))
		printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (1 != ($tmp = mysqli_num_rows($res)))
		printf("[018] Expecting 1 rows, mysqli_num_rows() reports %d rows. [%d] %s\n",
			$tmp, mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if (empty($row['id']) || empty($row['label']) || ($row['id'] != 1))
		printf("[020] Record seems to be incomplete\n");

	if ($blob != $row['label'])
		printf("[021] Blob value has not been stored properly!\n");

	if (NULL !== ($tmp = @mysqli_stmt_send_long_data($stmt, '')))
		printf("[022] Expecting NULL, got %s/%s\n");

	/* Check that the function alias exists. It's a deprecated function,
	but we have not announce the removal so far, therefore we need to check for it */
	if (!is_null($tmp = @mysqli_stmt_send_long_data()))
		printf("[023] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_stmt_send_long_data(): Invalid parameter number in %s on line %d
done!
