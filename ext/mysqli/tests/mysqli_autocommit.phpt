--TEST--
mysqli_autocommit()
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifemb.inc');
	require_once('connect.inc');
	require_once('skipifconnectfailure.inc');

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		die(sprintf("skip Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket));
	}

	if (!have_innodb($link))
		die(sprintf("Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_autocommit()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_autocommit($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_autocommit($link, $link, $link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[004] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}

	if (!is_bool($tmp = mysqli_autocommit($link, true)))
		printf("[005] Expecting boolean/any, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, 'SET AUTOCOMMIT = 0'))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT @@autocommit as auto_commit'))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if ($tmp['auto_commit'])
		printf("[008] Cannot turn off autocommit\n");

	if (true !== ($tmp = mysqli_autocommit($link, true)))
		printf("[009] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

	if (!$res = mysqli_query($link, 'SELECT @@autocommit as auto_commit'))
		printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if (!$tmp['auto_commit'])
		printf("[011] Cannot turn on autocommit\n");

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'CREATE TABLE test(id INT) ENGINE = InnoDB')) {
		printf("[013] Cannot create test table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1)'))
		printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'ROLLBACK'))
		printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS num FROM test'))
		printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ((!$tmp = mysqli_fetch_assoc($res)) || (1 != $tmp['num']))
		printf("[17] Expecting 1 row in table test, found %d rows. [%d] %s\n",
			$tmp['num'], mysqli_errno($link), mysqli_error($link));

	mysqli_free_result($res);

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'SET AUTOCOMMIT = 1'))
		printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT @@autocommit as auto_commit'))
		printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	mysqli_free_result($res);
	if (!$tmp['auto_commit'])
		printf("[021] Cannot turn on autocommit\n");

	if (true !== ($tmp = mysqli_autocommit($link, false)))
		printf("[022] Expecting true, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, 'CREATE TABLE test(id INT) ENGINE = InnoDB')) {
		printf("[023] Cannot create test table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1)'))
		printf("[024] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'ROLLBACK'))
		printf("[025] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS num FROM test'))
		printf("[026] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	$tmp = mysqli_fetch_assoc($res);
	if (0 != $tmp['num'])
		printf("[27] Expecting 0 rows in table test, found %d rows\n", $tmp['num']);
	mysqli_free_result($res);

	if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1)'))
		printf("[028] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'COMMIT'))
		printf("[029] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, 'SELECT COUNT(*) AS num FROM test'))
		printf("[030] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ((!$tmp = mysqli_fetch_assoc($res)) || (1 != $tmp['num']))
		printf("[31] Expecting 1 row in table test, found %d rows. [%d] %s\n",
			$tmp['num'], mysqli_errno($link), mysqli_error($link));
	mysqli_free_result($res);

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[032] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_close($link);

	if (false !== ($tmp = @mysqli_autocommit($link, false)))
		printf("[033] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
done!
