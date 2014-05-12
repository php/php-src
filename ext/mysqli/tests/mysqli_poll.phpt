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

		if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
			printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		return $link;
	}

	if (!$link = get_connection())
		printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	if (NULL !== ($tmp = @mysqli_poll()))
		printf("[002] Expecting NULL got %s\n", var_export($tmp, true));

	$l = array($link);
	if (NULL !== ($tmp = @mysqli_poll($l)))
		printf("[003] Expecting NULL got %s\n", var_export($tmp, true));

	$l = array($link); $n = NULL;
	if (NULL !== ($tmp = @mysqli_poll($l, $n)))
		printf("[004] Expecting NULL got %s\n", var_export($tmp, true));

	$l = array($link); $n = NULL;
	if (NULL !== ($tmp = @mysqli_poll($l, $n, $n)))
		printf("[005] Expecting NULL got %s\n", var_export($tmp, true));

	$l = array($link); $e = NULL; $r = NULL;
	if (NULL !== ($tmp = @mysqli_poll($l, $e, $r, -1)))
		printf("[007] Expecting boolean/false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	$l = array($link); $e = NULL; $r = NULL;
	if (NULL !== ($tmp = @mysqli_poll($l, $e, $r, 0, -1)))
		printf("[008] Expecting boolean/false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	$read = $error = $reject = array($link);
	if (0 !== ($tmp = (mysqli_poll($read, $error, $reject, 0, 1))))
		printf("[009] Expecting int/0 got %s/%s\n", gettype($tmp), var_export($tmp, true));

	$read = $error = $reject = array($link);
	if (false !== ($tmp = (mysqli_poll($read, $error, $reject, -1, 1))))
		printf("[010] Expecting false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	$read = $error = $reject = array($link);
	if (false !== ($tmp = (mysqli_poll($read, $error, $reject, 0, -1))))
		printf("[011] Expecting false got %s/%s\n", gettype($tmp), var_export($tmp, true));

	function poll_async($offset, $link, $links, $errors, $reject, $exp_ready, $use_oo_syntax) {

		if ($exp_ready !== ($tmp = mysqli_poll($links, $errors, $reject, 0, 1000)))
			printf("[%03d + 1] There should be %d links ready to read from, %d ready\n",
				$exp_ready, $tmp);

		foreach ($links as $mysqli) {
			if ($use_oo_syntax) {
				$res = $mysqli->reap_async_query();
			} else {
				$res = mysqli_reap_async_query($mysqli);
			}
			if (is_object($res)) {
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
	poll_async(12, $link, $links, $errors, $reject, 0, false);
	mysqli_close($link);

	$link = get_connection();
	$links = array($link);
	$errors = array($link);
	$reject = array($link);
	poll_async(13, $link, $links, $errors, $reject, 0, true);
	mysqli_close($link);

	// Connections on which no query has been send - 2
	// Difference: pass $links twice
	$link = get_connection();
	$links = array($link, $link);
	$errors = array($link, $link);
	$reject = array();
	poll_async(14, $link, $links, $errors, $reject, 0, false);

	// Connections on which no query has been send - 3
	// Difference: pass two connections
	$link = get_connection();
	$links = array($link, get_connection());
	$errors = array($link, $link);
	$reject = array();
	poll_async(15, $link, $links, $errors, $reject, 0, false);

	// Reference mess...
	$link = get_connection();
	$links = array($link);
	$errors = array($link);
	$ref_errors =& $errors;
	$reject = array();
	poll_async(16, $link, $links, $ref_errors, $reject, 0, false);

	print "done!";
?>
--EXPECTF--

Warning: mysqli_poll(): Negative values passed for sec and/or usec in %s on line %d

Warning: mysqli_poll(): Negative values passed for sec and/or usec in %s on line %d
[012 + 6] Rejecting thread %d: 0/
[013 + 6] Rejecting thread %d: 0/
[014 + 6] Rejecting thread %d: 0/
[014 + 6] Rejecting thread %d: 0/
[015 + 6] Rejecting thread %d: 0/
[015 + 6] Rejecting thread %d: 0/
[016 + 6] Rejecting thread %d: 0/
done!