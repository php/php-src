--TEST--
mysqli_begin_transaction()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die(sprintf("Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
	die(sprintf("Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
	require_once("connect.inc");
	/* {{{ proto bool mysqli_begin_transaction(object link, [int flags [, string name]]) */
	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_begin_transaction()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_begin_transaction($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_begin_transaction($link, $link)))
		printf("[003] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[004] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!is_null($tmp = @mysqli_begin_transaction($link, $link)))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_begin_transaction($link, 0, $link)))
		printf("[006] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_begin_transaction($link, 0, "mytrx", $link)))
		printf("[007] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test'))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'CREATE TABLE test(id INT) ENGINE = InnoDB'))
		printf("[009] Cannot create test table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (true !== ($tmp = mysqli_autocommit($link, true)))
		printf("[010] Cannot turn on autocommit, expecting true, got %s/%s\n", gettype($tmp), $tmp);

	/* overrule autocommit */
	if (true !== ($tmp = mysqli_begin_transaction($link)))
		printf("[011] Got %s - [%d] %s\n", var_dump($tmp, true), mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'INSERT INTO test(id) VALUES (1)'))
		printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$tmp = mysqli_rollback($link);
	if ($tmp !== true)
		printf("[013] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	/* empty */
	$res = mysqli_query($link, "SELECT * FROM test");
	var_dump($res->fetch_assoc());

	/* valid flags */
	$flags = array(
		MYSQLI_TRANS_START_WITH_CONSISTENT_SNAPSHOT,
		MYSQLI_TRANS_START_READ_WRITE,
		MYSQLI_TRANS_START_READ_ONLY,
		MYSQLI_TRANS_COR_AND_CHAIN, 
		MYSQLI_TRANS_COR_AND_NO_CHAIN, 
		MYSQLI_TRANS_COR_RELEASE, 
		MYSQLI_TRANS_COR_NO_RELEASE);

	/* just coverage */
	foreach ($flags as $flag) {
		if (!mysqli_begin_transaction($link, $flag, sprintf("flag %d", $flag))) {
			printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		}
		if (!mysqli_query($link, 'SELECT * FROM test') ||
			!mysqli_rollback($link)) {
			printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		}
	}

	/* does it really set a flag? */
	if (mysqli_get_server_version($link) >= 50600) {
		if (!mysqli_begin_transaction($link, MYSQLI_TRANS_START_READ_ONLY, sprintf("flag %d", $flag))) {
			printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		}
		if (!mysqli_query($link, "INSERT INTO test(id) VALUES (2)") ||
			!mysqli_commit($link)) {
			printf("[017] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		} else {
			$res = mysqli_query($link, "SELECT id FROM test WHERE id = 2");
			var_dump($res->fetch_assoc());
		}
	}
	
	/* invalid flag */
	do {
		$invalid_flag = mt_rand(0, 10000);
	} while (isset(array_flip($flags)[$invalid_flag]));
	/* we may or may not hit an invalid combination provoking a SQL error */
	if (!mysqli_begin_transaction($link, $invalid_flag, sprintf("flag %d", $invalid_flag))) {
		printf("[018] invalid_flag = %d [%d] %s\n", $invalid_flag, mysqli_errno($link), mysqli_error($link));
	} else {
		printf("[018] invalid_flag = %d [%d] %s\n", $invalid_flag, mysqli_errno($link), mysqli_error($link));
	}
	if (!mysqli_begin_transaction($link, -1)) {
			printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	/* does it like stupid names? */
	if (!$link->begin_transaction(MYSQLI_TRANS_START_READ_WRITE, "*/trick me?\n\0"))
		printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	/* does it like stupid names? */
	if (!$link->begin_transaction(MYSQLI_TRANS_START_READ_WRITE, "az09"))
		printf("[021] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
NULL
[017] [1792] %s
[018] invalid_flag = %d [%d]%A

Warning: mysqli_begin_transaction(): Invalid value for parameter flags (-1) in /work/dev/php/php-5.5/ext/mysqli/tests/mysqli_begin_transaction.php on line 98
[019] [%d]%A

Warning: mysqli::begin_transaction(): Transaction name truncated. Must be only [0-9A-Za-z\-_=]+ in /work/dev/php/php-5.5/ext/mysqli/tests/mysqli_begin_transaction.php on line 103
done!