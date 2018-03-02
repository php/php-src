--TEST--
mysqli_affected_rows()
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

	if (!is_null($tmp = @mysqli_affected_rows()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_affected_rows($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_affected_rows($link, $link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[004] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (0 !== ($tmp = mysqli_affected_rows($link)))
		printf("[005] Expecting int/0, got %s/%s. [%d] %s\n",
			gettype($tmp), $tmp, mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'CREATE TABLE test(id INT, label CHAR(1), PRIMARY KEY(id)) ENGINE = ' . $engine))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (1, 'a')"))
		printf("[008] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (1 !== ($tmp = mysqli_affected_rows($link)))
		printf("[010] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	// ignore INSERT error, NOTE: command line returns 0, affected_rows returns -1 as documented
	mysqli_query($link, "INSERT INTO test(id, label) VALUES (1, 'a')");
	if (-1 !== ($tmp = mysqli_affected_rows($link)))
		printf("[011] Expecting int/-1, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (1, 'a') ON DUPLICATE KEY UPDATE id = 4"))
		printf("[012] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (2 !== ($tmp = mysqli_affected_rows($link)))
		printf("[013] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "INSERT INTO test(id, label) VALUES (2, 'b'), (3, 'c')"))
		printf("[014] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (2 !== ($tmp = mysqli_affected_rows($link)))
		printf("[015] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "INSERT IGNORE INTO test(id, label) VALUES (1, 'a')")) {
		printf("[016] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));
	}

	if (1 !== ($tmp = mysqli_affected_rows($link)))
		printf("[017] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "INSERT INTO test(id, label) SELECT id + 10, label FROM test"))
		printf("[018] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (4 !== ($tmp = mysqli_affected_rows($link)))
		printf("[019] Expecting int/4, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "REPLACE INTO test(id, label) values (4, 'd')"))
		printf("[020] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (2 !== ($tmp = mysqli_affected_rows($link)))
		printf("[021] Expecting int/2, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "REPLACE INTO test(id, label) values (5, 'e')"))
		printf("[022] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (1 !== ($tmp = mysqli_affected_rows($link)))
		printf("[023] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "UPDATE test SET label = 'a' WHERE id = 2"))
		printf("[024] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));

	if (1 !== ($tmp = mysqli_affected_rows($link)))
		printf("[025] Expecting int/1, got %s/%s\n", gettype($tmp), $tmp);

	$charsets = array('utf8');
	foreach ($charsets as $k => $charset) {
		if (!($res = mysqli_query($link, sprintf("SHOW CHARACTER SET LIKE '%s'", $charset))))
			continue;
		mysqli_free_result($res);
		if (true !== ($tmp = mysqli_set_charset($link, $charset)))
			printf("[026] Expecting boolean/true got %s/%s\n",
				gettype($tmp), $tmp);
		if (0 !== ($tmp = mysqli_affected_rows($link)))
			printf("[027] Expecting int/0 got %s/%s\n", gettype($tmp), $tmp);
	}

	if (!mysqli_query($link, "UPDATE test SET label = 'a' WHERE id = 2")) {
		printf("[028] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));
	}

	if (0 !== ($tmp = mysqli_affected_rows($link)))
		printf("[029] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, "UPDATE test SET label = 'a' WHERE id = 100")) {
		printf("[030] [%d] %s\n",  mysqli_errno($link), mysqli_error($link));
	}

	if (0 !== ($tmp = mysqli_affected_rows($link)))
		printf("[031] Expecting int/0, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[032] [%d] %s\n", mysqli_errno($link), mysqli_error($link));


	mysqli_close($link);

	if (false !== ($tmp = @mysqli_affected_rows($link)))
		printf("[033] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECT--
done!
