--TEST--
int mysqli_poll() simple
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

		if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
			printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		return $link;
	}

	if (!$link = get_connection())
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (NULL !== ($tmp = @mysqli_poll()))
		printf("[002] Expecting NULL got %s\n", var_export($tmp, true));

	if (NULL !== ($tmp = @mysqli_poll(array($link))))
		printf("[003] Expecting NULL got %s\n", var_export($tmp, true));

	if (NULL !== ($tmp = @mysqli_poll(array($link), NULL)))
		printf("[004] Expecting NULL got %s\n", var_export($tmp, true));

	if (NULL !== ($tmp = @mysqli_poll(array($link), NULL, NULL)))
		printf("[005] Expecting NULL got %s\n", var_export($tmp, true));

	$int_val = 43;
	$myerrors = &$int_val;

	if (NULL !== ($tmp = @mysqli_poll(array($link), $myerrors, NULL, 1)))
		printf("[006] Expecting NULL got %s\n", var_export($tmp, true));

	if (NULL !== ($tmp = @mysqli_poll(array($link), NULL, NULL, -1)))
		printf("[007] Expecting boolean/false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	if (NULL !== ($tmp = @mysqli_poll(array($link), NULL, NULL, 0, -1)))
		printf("[008] Expecting boolean/false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	if (0 !== ($tmp = (mysqli_poll(array($link), array($link), array($link), 0, 1))))
		printf("[009] Expecting int/0 got %s/%s\n", gettype($tmp), var_export($tmp, true));


	function poll_async($offset, $links, $errors, $reject, $exp_ready) {

		if ($exp_ready !== ($tmp = mysqli_poll($links, $errors, $reject, 0, 1000)))
			printf("[%03d + 1] There should be %d links ready to read from, %d ready\n",
				$exp_ready, $tmp);

		foreach ($links as $mysqli) {
			if (is_object($res = mysqli_reap_async_query($mysqli))) {
				printf("[%03d + 2] Can fetch resultset although no query has been run!\n", $offset);
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
	$links = array($link);
	$errors = array($link);
	$reject = array($link);
	poll_async(10, $links, $errors, $reject, 0);
	mysqli_close($link);

	// Connections on which no query has been send - 2
	// Difference: pass $links twice
	$link = get_connection();
	$links = array($link, $link);
	$errors = array($link, $link);
	$reject = array();
	poll_async(11, $links, $errors, $reject, 0);

	// Connections on which no query has been send - 3
	// Difference: pass two connections
	$link = get_connection();
	$links = array($link, get_connection());
	$errors = array($link, $link);
	$reject = array();
	poll_async(12, $links, $errors, $reject, 0);

	// Reference mess...
	$link = get_connection();
	$links = array($link);
	$errors = array($link);
	$ref_errors =& $errors;
	$reject = array();
	poll_async(13, $links, $ref_errors, $reject, 0);

	print "done!";
?>
--EXPECTF--
[010 + 6] Rejecting thread %d: 0/
[011 + 6] Rejecting thread %d: 0/
[011 + 6] Rejecting thread %d: 0/
[012 + 6] Rejecting thread %d: 0/
[012 + 6] Rejecting thread %d: 0/
[013 + 6] Rejecting thread %d: 0/
done!
