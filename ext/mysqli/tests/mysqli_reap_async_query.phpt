--TEST--
mysqli_reap_async_query()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');
require_once('skipifconnectfailure.inc');

if (!$IS_MYSQLND)
	die("skip mysqlnd only feature, compile PHP using --with-mysqli=mysqlnd");
?>
--FILE--
<?php
	require_once('connect.inc');

	function get_connection() {
		global $host, $user, $passwd, $db, $port, $socket;

		if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
			printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		return $link;
	}

	if (!$link = get_connection())
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (NULL !== ($tmp = @mysqli_reap_async_query()))
		printf("[002] Expecting NULL got %s\n", var_export($tmp, true));

	$l = array($link);
	if (NULL !== ($tmp = @mysqli_reap_async_query($l)))
		printf("[003] Expecting NULL got %s\n", var_export($tmp, true));

	if (NULL !== ($tmp = @mysqli_reap_async_query($link, $link)))
		printf("[004] Expecting NULL got %s\n", var_export($tmp, true));


	function poll_async($offset, $link, $links, $errors, $reject, $exp_ready, $use_oo_syntax) {

		if ($exp_ready !== ($tmp = mysqli_poll($links, $errors, $reject, 0, 2000)))
			printf("[%03d + 1] There should be %d links ready to read from, %d ready\n",
				$offset, $exp_ready, $tmp);

		foreach ($links as $mysqli) {
			if ($use_oo_syntax) {
				$res = $mysqli->reap_async_query();
			} else {
				$res = mysqli_reap_async_query($mysqli);
			}
			if (is_object($res)) {
				printf("[%03d + 2] %s\n", $offset, var_export($res->fetch_assoc(), true));
			} else if (mysqli_errno($mysqli) > 0) {
				printf("[%03d + 3] Error indicated through links array: %d/%s",
					$offset, mysqli_errno($mysqli), mysqli_error($mysqli));
			} else {
				printf("[%03d + 4] Cannot fetch and no error set - non resultset query (no SELECT)!\n", $offset);
			}
		}

		foreach ($errors as $mysqli)
			printf("[%03d + 5] Error on %d: %d/%s\n",
				$offset, mysqli_thread_id($mysqli), mysqli_errno($mysqli), mysqli_error($mysqli));

		foreach ($reject as $mysqli)
			printf("[%03d + 6] Rejecting thread %d: %d/%s\n",
				$offset, mysqli_thread_id($mysqli), mysqli_errno($mysqli), mysqli_error($mysqli));

	}

	// Connections on which no query has been send - 1
	$link = get_connection();
	$link->query("SELECT 1 AS _one", MYSQLI_ASYNC | MYSQLI_STORE_RESULT);
	$links = array($link);
	$errors = array($link);
	$reject = array($link);
	poll_async(12, $link, $links, $errors, $reject, 1, false);
	mysqli_close($link);

	$link = get_connection();
	$link->query("SELECT 2 AS _two", MYSQLI_ASYNC | MYSQLI_USE_RESULT);
	$links = array($link);
	$errors = array($link);
	$reject = array($link);
	poll_async(13, $link, $links, $errors, $reject, 1, true);
	mysqli_close($link);

	print "done!";
?>
--EXPECT--
[012 + 2] array (
  '_one' => '1',
)
[013 + 2] array (
  '_two' => '2',
)
done!
