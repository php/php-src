--TEST--
mysqli_pconnect() - reusing/caching persistent connections - TODO
--SKIPIF--
<?php
die("skip TODO - we need to add a user level way to check if CHANGE_USER gets called by pconnect");

require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

if (!stristr(mysqli_get_client_info(), 'mysqlnd'))
	die("skip: only available in mysqlnd");
?>
--INI--
mysqli.allow_persistent=1
mysqli.max_persistent=2
mysqli.max_links=2
--FILE--
<?php
	require_once("connect.inc");

	$host = 'p:' . $host;
	if (!$link1 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket, mysqli_connect_errno(), mysqli_connect_error());
	}
	if (!mysqli_query($link1, 'SET @pcondisabled = "Connection 1"'))
		printf("[002] Cannot set user variable to check if we got the same persistent connection, [%d] %s\n",
			mysqli_errno($link1), mysqli_error($link1));

	if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket, mysqli_connect_errno(), mysqli_connect_error());
	}

	if (!$res = mysqli_query($link1, 'SELECT @pcondisabled AS _test'))
		printf("[004] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	printf("Connection 1 - SELECT @pcondisabled -> '%s'\n", $row['_test']);
	mysqli_free_result($res);

	if (!$res = mysqli_query($link2, 'SELECT @pcondisabled AS _test'))
		printf("[005] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	printf("Connection 2 (no reuse) - SELECT @pcondisabled -> '%s'\n", $row['_test']);
	$thread_id = mysqli_thread_id($link2);
	printf("Connection 2 (no reuse) - Thread ID -> '%s'\n", $thread_id);
	mysqli_free_result($res);

	if (!mysqli_query($link2, 'SET @pcondisabled = "Connection 2"'))
		printf("[006] Cannot set user variable to check if we got the same persistent connection, [%d] %s\n",
			mysqli_errno($link2), mysqli_error($link2));

	if (!$res = mysqli_query($link2, 'SELECT @pcondisabled AS _test'))
		printf("[007] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	printf("Connection 2 - SELECT @pcondisabled -> '%s'\n", $row['_test']);
	mysqli_free_result($res);

	mysqli_close($link2);

	/* reuse of existing persistent connection expected! */
	if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[008] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket, mysqli_connect_errno(), mysqli_connect_error());
	}

	if (!$res = mysqli_query($link2, 'SELECT @pcondisabled AS _test'))
		printf("[009] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	printf("Connection 2 (reuse) - SELECT @pcondisabled -> '%s'\n", $row['_test']);
	$thread_id_reuse = mysqli_thread_id($link2);
	printf("Connection 2 (reuse) - Thread ID -> '%s'\n", $thread_id_reuse);
	mysqli_free_result($res);

	if ($thread_id != $thread_id_reuse)
		printf("[010] Seems as if we have got a new connection, connections should have been cached and reused!\n");

	mysqli_close($link1);
	mysqli_close($link2);
	print "done!";
?>
--EXPECTF--
Connection 1 - SELECT @pcondisabled -> 'Connection 1'
Connection 2 (no reuse) - SELECT @pcondisabled -> ''
Connection 2 (no reuse) - Thread ID -> '%d'
Connection 2 - SELECT @pcondisabled -> 'Connection 2'
Connection 2 (reuse) - SELECT @pcondisabled -> 'Connection 2'
Connection 2 (reuse) - Thread ID -> '%d'
done!