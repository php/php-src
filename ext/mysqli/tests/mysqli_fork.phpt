--TEST--
Forking a child and using the same connection.
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!function_exists('pcntl_fork'))
	die("skip Process Control Functions not available");

if (!function_exists('posix_getpid'))
	die("skip POSIX functions not available");

require_once('connect.inc');
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
	die(sprintf("Cannot connect, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

if (!have_innodb($link))
	die(sprintf("Needs InnoDB support, [%d] %s", $link->errno, $link->error));
?>
--FILE--
<?php
	require_once("table.inc");

	$res = mysqli_query($link, "SELECT 'dumped by the parent' AS message");
	$pid = pcntl_fork();
	switch ($pid) {
		case -1:
			printf("[001] Cannot fork child");
			break;

		case 0:
			/* child */
			exit(0);
			break;

		default:
			/* parent */
			$status = null;
			$wait_id = pcntl_waitpid($pid, $status);
			if (pcntl_wifexited($status) && (0 != ($tmp = pcntl_wexitstatus($status)))) {
				printf("Exit code: %s\n", (pcntl_wifexited($status)) ? pcntl_wexitstatus($status) : 'n/a');
				printf("Signal: %s\n", (pcntl_wifsignaled($status)) ? pcntl_wtermsig($status) : 'n/a');
				printf("Stopped: %d\n", (pcntl_wifstopped($status)) ? pcntl_wstopsig($status) : 'n/a');
			}
			var_dump(mysqli_fetch_assoc($res));
			mysqli_free_result($res);
			break;
	}

	if (@mysqli_query($link, "SELECT id FROM test WHERE id = 1"))
		printf("[003] Expecting error and closed connection, child exit should have closed connection\n");
	else if ((($errno = mysqli_errno($link)) == 0) || ('' == ($error = mysqli_error($link))))
		printf("[004] Expecting error string and error code from MySQL, got errno = %s/%s, error = %s/%s\n",
			gettype($errno), $errno, gettype($error), $error);

	mysqli_close($link);
	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[005] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	/* non trivial tests require a message list for parent-child communication */
	if (!mysqli_query($link, "DROP TABLE IF EXISTS messages"))
		printf("[006] [%d] %s\n", mysqli_error($link), mysqli_errno($link));

	if (!mysqli_query($link, "CREATE TABLE messages(
		msg_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
		msg_time TIMESTAMP,
		pid INT NOT NULL,
		sender ENUM('child', 'parent') NOT NULL,
		msg TEXT) ENGINE = InnoDB"))
		printf("[007] [%d] %s\n", mysqli_error($link), mysqli_errno($link));

	mysqli_autocommit($link, false);
	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id ASC LIMIT 3", MYSQLI_USE_RESULT))
		printf("[008] [%d] %s\n", mysqli_error($link), mysqli_errno($link));

	$pid = pcntl_fork();

	switch ($pid) {
		case -1:
			printf("[009] Cannot fork child");
			break;

		case 0:
			/* child */
			if (!($plink = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) || !mysqli_autocommit($plink, true))
				exit(mysqli_errno($plink));

			$sql = sprintf("INSERT INTO messages(pid, sender, msg) VALUES (%d, 'child', '%%s')", posix_getpid());
			if (!mysqli_query($plink, sprintf($sql, 'start')))
				exit(mysqli_errno($plink));

			$parent_sql = sprintf("SELECT msg_id, msg_time, msg FROM messages WHERE pid = %d  AND sender = 'parent' ORDER BY msg_id DESC LIMIT 1", posix_getppid());
			$msg_id = 0;
			while ($row = mysqli_fetch_assoc($res)) {
				/* send row to parent */
				ob_start();
				var_dump($row);
				$tmp = ob_get_contents();
				ob_end_clean();
				if (!mysqli_query($plink, sprintf($sql, $tmp)))
					exit(mysqli_errno($plink));

				/* let the parent reply... */
				$start = time();
				do {
					usleep(100);
					if (!$pres = mysqli_query($plink, $parent_sql))
						continue;
					$tmp = mysqli_fetch_assoc($pres);
					mysqli_free_result($pres);
					if ($tmp['msg_id'] == $msg_id)
						/* no new message */
						continue;
					if ($tmp['msg'] == 'stop')
						break 2;
					$msg_id = $tmp['msg_id'];
					break;
				} while ((time() - $start) < 5);

			}

			if (!mysqli_query($plink, sprintf($sql, 'stop')) || !mysqli_commit($link))
				exit(mysqli_errno($plink));
			exit(0);
			break;

		default:
			/* parent */
			if (!$plink = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
					printf("[010] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
					$host, $user, $db, $port, $socket);

			$status = null;
			$start = time();
			$sql = sprintf("SELECT msg_id, msg_time, msg FROM messages WHERE pid = %d AND sender = 'child' ORDER BY msg_id DESC LIMIT 1", $pid);
			$parent_sql = sprintf("INSERT INTO messages (pid, sender, msg) VALUES (%d, 'parent', '%%s')", posix_getpid());
			$last_msg_id = 0;
			$num_rows = 0;
			do {
				$wait_id = pcntl_waitpid($pid, $status, WNOHANG);
				if ($pres = mysqli_query($plink, $sql)) {
					$row = mysqli_fetch_assoc($pres);
					if ($row['msg_id'] != $last_msg_id) {
						$last_msg_id = $row['msg_id'];
						switch ($row['msg']) {
							case 'start':
								break;
							case 'stop':
								break 2;
							default:
								/* client has started fetching rows */
								$client_row = $row['msg'];

								$num_rows++;
								if ($num_rows > 3) {
									printf("[011] Child has fetched more than three rows!\n");
									var_dump($client_row);
									if (!mysqli_query($plink, sprintf($parent_sql, 'stop'))) {
										printf("[012] Parent cannot inform child\n", mysqli_errno($plink), mysqli_error($plink));
									}
									break 2;
								}

								if (!$parent_row = mysqli_fetch_assoc($res)) {
									printf("[013] Parent cannot fetch row %d\n", $num_rows, mysqli_errno($link), mysqli_error($link));
									if (!mysqli_query($plink, sprintf($parent_sql, 'stop'))) {
										printf("[014] Parent cannot inform child\n", mysqli_errno($plink), mysqli_error($plink));
									}
									break 2;
								}

								ob_start();
								var_dump($parent_row);
								$parent_row = ob_get_contents();
								ob_end_clean();

								if ($parent_row != $client_row) {
									printf("[015] Child indicates different results than parent.\n");
									var_dump($child_row);
									var_dump($parent_row);
									if (!mysqli_query($plink, sprintf($parent_sql, 'stop'))) {
										printf("[016] Parent cannot inform child\n", mysqli_errno($plink), mysqli_error($plink));
									}
									break 2;
								}

								if (!mysqli_query($plink, sprintf($parent_sql, 'continue'))) {
									printf("[017] Parent cannot inform child to continue.\n", mysqli_errno($plink), mysqli_error($plink));
								}
								break;
						}
					}
					mysqli_free_result($pres);
				}
				usleep(100);
			} while (((time() - $start) < 5) && ($num_rows < 3));
			mysqli_close($plink);
			$wait_id = pcntl_waitpid($pid, $status);
			if (pcntl_wifexited($status) && (0 != ($tmp = pcntl_wexitstatus($status)))) {
				printf("Exit code: %s\n", (pcntl_wifexited($status)) ? pcntl_wexitstatus($status) : 'n/a');
				printf("Signal: %s\n", (pcntl_wifsignaled($status)) ? pcntl_wtermsig($status) : 'n/a');
				printf("Stopped: %d\n", (pcntl_wifstopped($status)) ? pcntl_wstopsig($status) : 'n/a');
			}
			break;
	}
	mysqli_free_result($res);
	mysqli_close($link);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[018] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (!$res = mysqli_query($link, "SELECT sender, msg FROM messages ORDER BY msg_id ASC"))
		printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	while ($row = mysqli_fetch_assoc($res))
		printf("%10s %s\n", $row['sender'], substr($row['msg'], 0, 5));
	mysqli_free_result($res);

	print "done!";
?>
--CLEAN--
<?php
require_once("connect.inc");
if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
   printf("[c001] [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

if (!mysqli_query($link, "DROP TABLE IF EXISTS test"))
	printf("[c002] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

if (!mysqli_query($link, "DROP TABLE IF EXISTS messages"))
	printf("[c003] Cannot drop table, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysqli_close($link);
?>
--EXPECTF--
array(1) {
  ["message"]=>
  string(20) "dumped by the parent"
}
     child start
     child array
    parent conti
     child array
    parent conti
     child array
    parent conti
     child stop
done!