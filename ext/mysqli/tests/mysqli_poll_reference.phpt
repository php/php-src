--TEST--
mysqli_poll() & references
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');
require_once('skipifconnectfailure.inc');

if (!$IS_MYSQLND)
	die("skip mysqlnd only feature, compile PHP using --with-mysqli=mysqlnd");

if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die("skip cannot connect");

if (mysqli_server_version($link) < 50012))
	die("skip Test needs SQL function SLEEP() available as of MySQL 5.0.12");

mysqli_close($link);
?>
--FILE--
<?php
	require_once('connect.inc');

	function get_connection() {
		global $host, $user, $passwd, $db, $port, $socket;

		if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
			printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		return $link;
	}


	$mysqli1 = get_connection();
	$mysqli2 = get_connection();

	var_dump(mysqli_query($mysqli1, "SELECT SLEEP(0.10)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));
	var_dump(mysqli_query($mysqli2, "SELECT SLEEP(0.20)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));

	$processed = $loops = 0;
	do {
		$loops++;
		if ($loops > 10) {
			printf("[002] The queries should have finished already\n");
			break;
		}
		// WARNING: All arrays point to the same object - this will give bogus results!
		// The behaviour is in line with stream_select(). Be warned, be careful.
		$links = $errors = $reject = array($mysqli1, $mysqli2);
		if (0 == ($ready = mysqli_poll($links, $errors, $reject, 0, 50000))) {
			continue;
		}

		foreach ($links as $link) {
			if ($res = mysqli_reap_async_query($link)) {
				mysqli_free_result($res);
			}
			$processed++;
		}
	} while ($processed < 2);

	mysqli_close($mysqli1);
	mysqli_close($mysqli2);

	$mysqli1 = get_connection();
	$mysqli2 = get_connection();

	var_dump(mysqli_query($mysqli1, "SELECT SLEEP(0.10)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));
	var_dump(mysqli_query($mysqli2, "SELECT SLEEP(0.20)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));

	$processed = $loops = 0;
	do {
		$loops++;
		if ($loops > 10) {
			printf("[003] The queries should have finished already\n");
			break;
		}
		// WARNING: All arrays point to the same object - this will give bogus results!
		$links = $errors = array($mysqli1, $mysqli2);
		$reject = array($mysqli1, $mysqli2);
		if (0 == ($ready = mysqli_poll($links, $errors, $reject, 0, 50000))) {
			continue;
		}
		foreach ($links as $link) {
			if ($res = mysqli_reap_async_query($link)) {
				mysqli_free_result($res);
			}
			$processed++;
		}
	} while ($processed < 2);

	mysqli_close($mysqli1);
	mysqli_close($mysqli2);

	$mysqli1 = get_connection();
	$mysqli2 = get_connection();

	var_dump(mysqli_query($mysqli1, "SELECT SLEEP(0.10)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));
	var_dump(mysqli_query($mysqli2, "SELECT SLEEP(0.20)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));

	$processed = $loops = 0;
	do {
		$loops++;
		if ($loops > 10) {
			printf("[004] The queries should have finished already\n");
			break;
		}
		// WARNING: All arrays point to the same object - this will give bogus results!
		$links = array($mysqli1, $mysqli2);
		$errors = $reject = array($mysqli1, $mysqli2);
		if (0 == ($ready = mysqli_poll($links, $errors, $reject, 0, 50000))) {
			continue;
		}
		foreach ($links as $link) {
			if ($res = mysqli_reap_async_query($link)) {
				mysqli_free_result($res);
			}
			$processed++;
		}
	} while ($processed < 2);

	mysqli_close($mysqli1);
	mysqli_close($mysqli2);

	// This is bogus code and bogus usage - OK to throw no errors!
	$mysqli1 = get_connection();
	$mysqli2 = get_connection();

	var_dump(mysqli_query($mysqli1, "SELECT SLEEP(0.10)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));
	$thread_id = mysqli_thread_id($mysqli2);
	printf("Connection %d should be rejected...\n", $thread_id);

	$processed = $loops = 0;
	do {
		$loops++;
		if ($loops > 10) {
			printf("[005] The queries should have finished already\n");
			break;
		}
		$links = $errors = $reject = array($mysqli1, $mysqli2);
		if (0 == ($ready = mysqli_poll($links, $errors, $reject, 0, 50000))) {
			continue;
		}
		// WARNING: Due to the reference issue none of these should ever fire!
		foreach ($reject as $links) {
			printf("Connection %d was rejected...\n", mysqli_thread_id($link));
			$processed++;
		}
		foreach ($errors as $links) {
			printf("Connection %d has an error...\n", mysqli_thread_id($link));
			$processed++;
		}
		foreach ($links as $link) {
			if ($res = mysqli_reap_async_query($link)) {
				mysqli_free_result($res);
				$processed++;
			}
		}
	} while ($processed < 2);

	mysqli_close($mysqli1);
	mysqli_close($mysqli2);

	$mysqli1 = get_connection();
	$mysqli2 = get_connection();

	var_dump(mysqli_query($mysqli1, "SELECT SLEEP(0.10)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));
	var_dump(mysqli_query($mysqli2, "SELECT SLEEP(0.20)", MYSQLI_ASYNC | MYSQLI_USE_RESULT));

	$processed = $loops = 0;
	$all = array($mysqli1, $mysqli2);
	do {
		$loops++;
		if ($loops > 10) {
			printf("[006] The queries should have finished already\n");
			break;
		}
		$links = $errors = $reject = $all;
		ob_start();
		if (0 == ($ready = mysqli_poll($links, $errors, $reject, 0, 50000))) {
			$tmp = ob_get_contents();
			ob_end_clean();
			if ($tmp != '') {
				printf("Expected error:\n%s\n", $tmp);
				break;
			}
			continue;
		}
		foreach ($links as $link) {
			if ($res = mysqli_reap_async_query($link)) {
				mysqli_free_result($res);
			}
			$processed++;
		}
	} while ($processed < 2);

	mysqli_close($mysqli1);
	mysqli_close($mysqli2);

	print "done!";
?>
--EXPECTF--
bool(true)
bool(true)
[002] The queries should have finished already
bool(true)
bool(true)
[003] The queries should have finished already
bool(true)
bool(true)
bool(true)
Connection %d should be rejected...
[005] The queries should have finished already
bool(true)
bool(true)
Expected error:

Warning: mysqli_poll(): No stream arrays were passed in %s on line %d

done!
