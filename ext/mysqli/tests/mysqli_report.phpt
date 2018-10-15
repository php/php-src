--TEST--
mysqli_report()
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

	if (NULL !== ($tmp = @mysqli_report()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_report(-1)))
		printf("[002] Expecting boolean/true even for invalid flags, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_report(MYSQLI_REPORT_ERROR)))
		printf("[003] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_report(MYSQLI_REPORT_STRICT)))
		printf("[004] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_report(MYSQLI_REPORT_INDEX)))
		printf("[005] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_report(MYSQLI_REPORT_ALL)))
		printf("[007] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (true !== ($tmp = mysqli_report(MYSQLI_REPORT_OFF)))
		printf("[008] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	/*
	Internal macro MYSQL_REPORT_ERROR
	*/
	mysqli_report(MYSQLI_REPORT_ERROR);

	mysqli_multi_query($link, "BAR; FOO;");
	mysqli_query($link, "FOO");
	mysqli_kill($link, -1);

	// mysqli_ping() cannot be tested, because one would need to cause an error inside the C function to test it
	mysqli_prepare($link, "FOO");
	mysqli_real_query($link, "FOO");
	if (@mysqli_select_db($link, "Oh lord, let this be an unknown database name"))
		printf("[009] select_db should have failed\n");
	// mysqli_store_result() and mysqli_use_result() cannot be tested, because one would need to cause an error inside the C function to test it


	// Check that none of the above would have caused any error messages if MYSQL_REPORT_ERROR would
	// not have been set. If that would be the case, the test would be broken.
	mysqli_report(MYSQLI_REPORT_OFF);

	mysqli_multi_query($link, "BAR; FOO;");
	mysqli_query($link, "FOO");
	mysqli_kill($link, -1);
	mysqli_prepare($link, "FOO");
	mysqli_real_query($link, "FOO");
	mysqli_select_db($link, "Oh lord, let this be an unknown database name");

	/*
	Internal macro MYSQL_REPORT_STMT_ERROR
	*/

	mysqli_report(MYSQLI_REPORT_ERROR);

	$stmt = mysqli_stmt_init($link);
	mysqli_stmt_prepare($stmt, "FOO");

	$stmt = mysqli_stmt_init($link);
	mysqli_stmt_prepare($stmt, "SELECT id FROM test WHERE id > ?");
	$id = 1;
	mysqli_kill($link, mysqli_thread_id($link));
	mysqli_stmt_bind_param($stmt, "i", $id);
	mysqli_stmt_close($stmt);
	mysqli_close($link);

	/* mysqli_stmt_execute() = mysql_stmt_execute cannot be tested from PHP */
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
			printf("[008] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	$stmt = mysqli_stmt_init($link);
	mysqli_stmt_prepare($stmt, "SELECT id FROM test WHERE id > ?");
	$id = 1;
	mysqli_stmt_bind_param($stmt, "i", $id);
	// mysqli_kill($link, mysqli_thread_id($link));
	mysqli_stmt_execute($stmt);
	mysqli_stmt_close($stmt);
	mysqli_close($link);

	/* mysqli_kill() "trick" does not work for any of the following because of an E_COMMANDS_OUT_OF_SYNC */
	/* mysqli_stmt_bind_result() = mysql_stmt_bind_result() cannot be tested from PHP */
	/* mysqli_stmt_fetch() = mysql_stmt_fetch() cannot be tested from PHP */
	/* mysqli_stmt_result_metadata() = mysql_stmt_result_metadata() cannot be tested from PHP */
	/* mysqli_stmt_store_result() = mysql_stmt_store_result() cannot be tested from PHP */

	// Check
	mysqli_report(MYSQLI_REPORT_OFF);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[010] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	$stmt = mysqli_stmt_init($link);
	mysqli_stmt_prepare($stmt, "FOO");

	$stmt = mysqli_stmt_init($link);
	mysqli_stmt_prepare($stmt, "SELECT id FROM test WHERE id > ?");
	$id = 1;
	mysqli_kill($link, mysqli_thread_id($link));
	mysqli_stmt_bind_param($stmt, "i", $id);
	mysqli_stmt_close($stmt);
	mysqli_close($link);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[011] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	$stmt = mysqli_stmt_init($link);
	mysqli_stmt_prepare($stmt, "SELECT id FROM test WHERE id > ?");
	$id = 1;
	mysqli_stmt_bind_param($stmt, "i", $id);
	mysqli_kill($link, mysqli_thread_id($link));
	mysqli_stmt_execute($stmt);
	mysqli_stmt_close($stmt);
	mysqli_close($link);

	/*
	MYSQLI_REPORT_STRICT

	MYSQLI_REPORT_STRICT --->
	php_mysqli_report_error() ->
		MYSQLI_REPORT_MYSQL_ERROR,
		MYSQLI_REPORT_STMT_ERROR ->
			already tested

	php_mysqli_throw_sql_exception() ->
		my_mysqli_real_connect()
		my_mysqli_connect()

	can't be tested: mysqli_query() via mysql_use_result()/mysql_store_result()
	*/
	mysqli_report(MYSQLI_REPORT_OFF);
	mysqli_report(MYSQLI_REPORT_STRICT);

	try {

		if ($link = my_mysqli_connect($host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket))
			printf("[012] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
				$host, $user . 'unknown_really', $db, $port, $socket);
		mysqli_close($link);

	} catch (mysqli_sql_exception $e) {
		printf("[013] %s\n", $e->getMessage());
	}

	try {
		if (!$link = mysqli_init())
			printf("[014] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

		if ($link = my_mysqli_real_connect($link, $host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket))
			printf("[015] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
				$host, $user . 'unknown_really', $db, $port, $socket);
		mysqli_close($link);
	} catch (mysqli_sql_exception $e) {
		printf("[016] %s\n", $e->getMessage());
	}

	/*
	MYSQLI_REPORT_INDEX --->
	mysqli_query()
	mysqli_stmt_execute()
	mysqli_prepare()
	mysqli_real_query()
	mysqli_store_result()
	mysqli_use_result()

	No test, because of to many prerequisites:
		- Server needs to be started with and
			--log-slow-queries --log-queries-not-using-indexes
		- query must cause the warning on all MySQL versions

	TODO:
	*/
	$log_slow_queries = false;
	$log_queries_not_using_indexes = false;
	mysqli_report(MYSQLI_REPORT_OFF);
	mysqli_report(MYSQLI_REPORT_INDEX);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[017] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (mysqli_get_server_version($link) <= 50600) {
		// this might cause a warning - no index used
		if (!$res = @mysqli_query($link, "SHOW VARIABLES LIKE 'log_slow_queries'"))
			printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		if (!$row = mysqli_fetch_assoc($res))
			printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		$log_slow_query = ('ON' == $row['Value']);

		if (mysqli_get_server_version($link) >= 50111) {
			// this might cause a warning - no index used
			if (!$res = @mysqli_query($link, "SHOW VARIABLES LIKE 'log_queries_not_using_indexes'"))
				printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

			if (!$row = mysqli_fetch_assoc($res))
				printf("[021] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

			$log_queries_not_using_indexes = ('ON' == $row['Value']);

			if ($log_slow_queries && $log_queries_not_using_indexes) {

				for ($i = 100; $i < 20000; $i++) {
					if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES ($i, 'z')"))
						printf("[022 - %d] [%d] %s\n", $i - 99, mysqli_errno($link), mysqli_error($link));
				}

				// this might cause a warning - no index used
				if (!$res = @mysqli_query($link, "SELECT id, label FROM test WHERE id = 1323"))
					printf("[023] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

				mysqli_free_result($res);
			}
		}
	}

	// Maybe we've provoked an index message, maybe not.
	// All we can do is make a few dummy calls to ensure that all codes gets executed which
	// checks the flag. Functions to check: mysqli_query() - done above,
	// mysqli_stmt_execute(), mysqli_prepare(), mysqli_real_query(), mysqli_store_result()
	// mysqli_use_result(), mysqli_thread_safe(), mysqli_thread_id()
	mysqli_report(MYSQLI_REPORT_OFF);
	mysqli_close($link);
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[024] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (!$stmt = mysqli_stmt_init($link))
		printf("[025] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_stmt_prepare($stmt, 'SELECT id, label FROM test'))
		printf("[026] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	if (!mysqli_stmt_execute($stmt))
		printf("[027] [%d] %s\n", mysqli_stmt_errno($stmt), mysqli_stmt_error($stmt));

	mysqli_stmt_close($stmt);

	if (!mysqli_real_query($link, 'SELECT label, id FROM test'))
		printf("[028] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_use_result($link))
		printf("[029] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if (!mysqli_real_query($link, 'SELECT label, id FROM test'))
		printf("[030]  [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_store_result($link))
		printf("[031]  [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if (!$stmt = mysqli_prepare($link, 'SELECT id * 3 FROM test'))
		printf("[032] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	else
		mysqli_stmt_close($stmt);

	if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (100, 'z')", MYSQLI_USE_RESULT) ||
			!mysqli_query($link, 'DELETE FROM test WHERE id > 50', MYSQLI_USE_RESULT))
		printf("[033] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$tmp = mysqli_thread_safe($link);
	$tmp = mysqli_thread_id($link);

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_multi_query(): (%d/%d): You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'BAR; FOO' at line 1 in %s on line %d

Warning: mysqli_query(): (%d/%d): You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'FOO' at line 1 in %s on line %d

Warning: mysqli_kill(): processid should have positive value in %s on line %d

Warning: mysqli_prepare(): (%d/%d): You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'FOO' at line 1 in %s on line %d

Warning: mysqli_real_query(): (%d/%d): You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'FOO' at line 1 in %s on line %d

Warning: mysqli_kill(): processid should have positive value in %s on line %d

Warning: mysqli_stmt_prepare(): (%d/%d): You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'FOO' at line 1 in %s on line %d
[013] Access denied for user '%s'@'%s' (using password: YES)
[016] Access denied for user '%s'@'%s' (using password: YES)
done!
