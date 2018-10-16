--TEST--
mysqli_poll() & INSERT SELECT
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
	require_once('table.inc');

	function get_connection() {
		global $host, $user, $passwd, $db, $port, $socket;

		if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
			printf("[001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
		return $link;
	}


	// Note: some queries will fail! They are supposed to fail.
	$queries = array(
			'CREATE TABLE IF NOT EXISTS bogus(id INT)',
			'SET @a = 1',
			'SELECT * FROM test ORDER BY id ASC LIMIT 2',
			"INSERT INTO test(id, label) VALUES (100, 'z')",
			'SELECT * FROM test ORDER BY id ASC LIMIT 2',
			'SELECT',
			'UPDATE test SET id = 101 WHERE id > 3',
			'UPDATE_FIX test SET id = 101 WHERE id > 3',
			'DROP TABLE IF EXISTS bogus',
			'DELETE FROM test WHERE id = @a',
			'DELETE FROM test WHERE id = 1',
	);

	$link = get_connection();
	$have_proc = false;
	mysqli_real_query($link, "DROP PROCEDURE IF EXISTS p");
	if (mysqli_real_query($link, 'CREATE PROCEDURE p(IN ver_in VARCHAR(25), OUT ver_out VARCHAR(25)) BEGIN SELECT ver_in INTO ver_out; END;')) {
			$have_proc = true;
			$queries[] = "CALL p('myversion', @version)";
	}
	mysqli_close($link);

	$links = array();
	for ($i = 0; $i < count($queries); $i++) {

		$link = get_connection();

		if (true !== ($tmp = mysqli_query($link, $queries[$i], MYSQLI_ASYNC |  MYSQLI_USE_RESULT)))
			printf("[002] Expecting true got %s/%s\n", gettype($tmp), var_export($tmp, true));

		// WARNING KLUDGE NOTE
		// Add a tiny delay to ensure that queries get executed in a certain order
		// If your MySQL server is very slow the test may randomly fail!
		usleep(20000);

		$links[mysqli_thread_id($link)] = array(
			'query' => $queries[$i],
			'link' => $link,
			'processed' => false,
		);
	}

	$saved_errors = array();
	do {
		$poll_links = $poll_errors = $poll_reject = array();
		foreach ($links as $thread_id => $link) {
			if (!$link['processed']) {
				$poll_links[] = $link['link'];
				$poll_errors[] = $link['link'];
				$poll_reject[] = $link['link'];
			}
		}
		if (0 == count($poll_links))
			break;

		if (0 === ($num_ready = mysqli_poll($poll_links, $poll_errors, $poll_reject, 0, 200000)))
			continue;

		if (!empty($poll_errors)) {
			die(var_dump($poll_errors));
		}

		if (FALSE === $num_ready) {
			die("Some mysqli indicated error");
		}

		foreach ($poll_links as $link) {
			$thread_id = mysqli_thread_id($link);
			$links[$thread_id]['processed'] = true;

			if (is_object($res = mysqli_reap_async_query($link))) {
				// result set object
				while ($row = mysqli_fetch_assoc($res)) {
					// eat up all results
					;
				}
				mysqli_free_result($res);
			} else {
				// either there is no result (no SELECT) or there is an error
				if (mysqli_errno($link) > 0) {
					$saved_errors[$thread_id] = mysqli_errno($link);
					printf("[003] '%s' caused %d\n", $links[$thread_id]['query'],	mysqli_errno($link));
				}
			}
		}

	} while (true);

	// Checking if all lines are still usable
	foreach ($links as $thread_id => $link) {
		if (isset($saved_errors[$thread_id]) &&
			$saved_errors[$thread_id] != mysqli_errno($link['link'])) {
			printf("[004] Error state not saved for query '%s', %d != %d\n", $link['query'],
					$saved_errors[$thread_id], mysqli_errno($link['link']));
		}

		if (!$res = mysqli_query($link['link'], 'SELECT * FROM test WHERE id = 100'))
			printf("[005] Expecting true got %s/%s\n", gettype($tmp), var_export($tmp, true));
		if (!$row = mysqli_fetch_row($res))
			printf("[006] Expecting true got %s/%s\n", gettype($tmp), var_export($tmp, true));

		mysqli_free_result($res);
	}

	if ($res = mysqli_query($link['link'], "SELECT * FROM test WHERE id = 100")) {
		$row = mysqli_fetch_assoc($res);
		var_dump($row);
		mysqli_free_result($res);
	}

	if ($have_proc && ($res = mysqli_query($link['link'], "SELECT @version as _version"))) {
		$row = mysqli_fetch_assoc($res);
		if ($row['_version'] != 'myversion') {
			printf("[007] Check procedures\n");
		}
		mysqli_free_result($res);
	}

	foreach ($links as $link)
		mysqli_close($link['link']);

	$link = get_connection();
	if (!mysqli_query($link, 'SELECT 1', MYSQLI_ASYNC))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (!mysqli_query($link, 'SELECT 1', MYSQLI_ASYNC))
		printf("[009] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_close($link);

	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

if (!mysqli_query($link, "DROP TABLE IF EXISTS bogus"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_query($link, "DROP PROCEDURE IF EXISTS p");

mysqli_close($link);
?>
--EXPECTF--
[003] 'SELECT' caused 1064
[003] 'UPDATE test SET id = 101 WHERE id > 3' caused 1062
[003] 'UPDATE_FIX test SET id = 101 WHERE id > 3' caused 1064
array(2) {
  ["id"]=>
  string(3) "100"
  ["label"]=>
  string(1) "z"
}
[009] [2014] %s
done!
