--TEST--
Trying implicit reconnect after wait_timeout and KILL using mysqli_ping()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysqli.reconnect=0
--FILE--
<?php
	require_once("connect.inc");
	require_once("table.inc");

	if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot create second database connection, [%d] %s\n",
			mysqli_connect_errno(), mysqli_connect_error());

	$thread_id_timeout = mysqli_thread_id($link);
	$thread_id_control = mysqli_thread_id($link2);

	if (!$res = mysqli_query($link2, "SHOW FULL PROCESSLIST"))
		printf("[002] Cannot get full processlist, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link));

	$running_threads = array();
	while ($row = mysqli_fetch_assoc($res))
		$running_threads[$row['Id']] = $row;
	mysqli_free_result($res);

	if (!isset($running_threads[$thread_id_timeout]) ||
			!isset($running_threads[$thread_id_control]))
		printf("[003] Processlist is borked, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link));

	if (!mysqli_query($link, "SET SESSION wait_timeout = 2"))
		printf("[004] Cannot set wait_timeout, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, "SHOW VARIABLES LIKE 'wait_timeout'"))
		printf("[005] Cannot check if wait_timeout has been set, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[006] Cannot get wait_timeout, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));
	mysqli_free_result($res);

	if ($row['Value'] != 2)
		printf("[007] Failed setting the wait_timeout, test will not work, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	// after 2+ seconds the server should kill the connection
	sleep(3);

	if (!$res = mysqli_query($link2, "SHOW FULL PROCESSLIST"))
		printf("[008] Cannot get full processlist, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link));

	$running_threads = array();
	while ($row = mysqli_fetch_assoc($res))
		$running_threads[$row['Id']] = $row;
	mysqli_free_result($res);

	if (isset($running_threads[$thread_id_timeout]))
		printf("[009] Server should have killed the timeout connection, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link));

	if (false !== @mysqli_ping($link))
		printf("[010] Reconnect should not have happened");

	if ($res = @mysqli_query($link, "SELECT DATABASE() as _dbname"))
		printf("[011] Executing a query should not be possible, connection should be closed, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	if (!$link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[012] Cannot create database connection, [%d] %s\n",
			mysqli_connect_errno(), mysqli_connect_error());

	$thread_id_timeout = mysqli_thread_id($link);
	/*
	  Don't test for the mysqli_query() return value here.
	  It is undefined if the server replies to the query and how.
	  For example, it seems that on Linux when connecting to MySQL 5.1,
	  the server always manages to send a full a reply. Whereas MySQl 5.5
	  may not. The behaviour is undefined. Any return value is fine.
	*/
	if ($IS_MYSQLND) {
		/*
		mysqlnd is a bit more verbose than libmysql. mysqlnd should print:
		Warning: mysqli_query(): MySQL server has gone away in %s on line %d

		Warning: mysqli_query(): Error reading result set's header in %d on line %d
		*/
		@mysqli_query($link, sprintf('KILL %d', $thread_id_timeout));
	} else {
		mysqli_query($link, sprintf('KILL %d', $thread_id_timeout));
	}
	// Give the server a second to really kill the other thread...
	sleep(1);

	if (!$res = mysqli_query($link2, "SHOW FULL PROCESSLIST"))
		printf("[014] Cannot get full processlist, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link));

	$running_threads = array();
	while ($row = mysqli_fetch_assoc($res))
		$running_threads[$row['Id']] = $row;
	mysqli_free_result($res);

	if (isset($running_threads[$thread_id_timeout]) ||
			!isset($running_threads[$thread_id_control]))
		printf("[015] Processlist is borked, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link));

	if (false !== ($tmp = @mysqli_ping($link)))
		printf("[016] Expecting boolean/false got %s/%s\n", gettype($tmp), $tmp);

	if ($res = @mysqli_query($link, "SELECT DATABASE() as _dbname"))
		printf("[017] Running a query should not be possible, connection should be gone, [%d] %s\n",
			mysqli_errno($link), mysqli_error($link));

	mysqli_close($link2);
	print "done!";
?>
--EXPECT--
done!
