--TEST--
mysqli_select_db()
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

	if (!is_null($tmp = @mysqli_select_db()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_select_db($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!is_null($tmp = @mysqli_select_db($link, $db, "foo")))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	/* does not make too much sense, unless we have access to at least one more database than $db */
	if (!mysqli_select_db($link, $db))
		printf("[005] Cannot select DB %s, [%d] %s\n", $db, mysqli_errno($link), mysqli_error($link));

	if (!$res = mysqli_query($link, "SELECT DATABASE() AS dbname"))
		printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[007] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ($row['dbname'] !== (string)$db)
		printf("[008] Expecting database '%s', found '%s'\n", $db, $row['dbname']);
	mysqli_free_result($res);

	if (mysqli_select_db($link, 'mysql')) {
		// Yippie, a second database to play with - that's great because mysqli_select_db
		// ($db) was done by mysqli__connect() already and the previous test
		// was quite useless
		if (!$res = mysqli_query($link, "SELECT DATABASE() AS dbname"))
			printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		if (!$row = mysqli_fetch_assoc($res))
			printf("[010] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		if (strtolower($row['dbname']) !== 'mysql')
			printf("[011] Expecting database 'mysql', found '%s'\n", $row['dbname']);
		mysqli_free_result($res);
	}

	mysqli_report(MYSQLI_REPORT_OFF);
	mysqli_select_db($link, 'I can not imagine that this database exists');
	mysqli_report(MYSQLI_REPORT_ERROR);
	mysqli_select_db($link, 'I can not imagine that this database exists');

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_select_db($link, $db)))
		printf("[012] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!\n";
?>
--EXPECTF--
Warning: mysqli_select_db(): (%d/%d): Unknown database '%s' in %s on line %d

Warning: mysqli_select_db(): Couldn't fetch mysqli in %s on line %d
done!