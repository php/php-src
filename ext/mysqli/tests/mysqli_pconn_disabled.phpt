--TEST--
mysqli_pconnect() - mysqli.allow_persistent = 0
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');

die("skip TODO - we need to add a user level way to check if CHANGE_USER gets called by pconnect");
?>
--INI--
mysqli.allow_persistent=0
mysqli.max_persistent=2
mysqli.max_links=2
--FILE--
<?php
	require_once("connect.inc");

	$host = 'p:' . $host;
	if (!$link1 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		// automatic downgrade to normal connections has failed
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket, mysqli_connect_errno(), mysqli_connect_error());
	}
	if (!mysqli_query($link1, "SET @pcondisabled = 'Connection 1'"))
		printf("[002] Cannot set user variable to check if we got the same persistent connection, [%d] %s\n",
			mysqli_errno($link1), mysqli_error($link1));

	if (!$link2 = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		// automatic downgrade to normal connections has failed
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s, [%d] %s\n",
			$host, $user, $db, $port, $socket, mysqli_connect_errno(), mysqli_connect_error());
	}

	if (!$res = mysqli_query($link1, 'SELECT @pcondisabled AS _test'))
		printf("[004] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	printf("Connecction 1 - SELECT @pcondisabled -> '%s'\n", $row['_test']);
	mysqli_free_result($res);

	if (!$res = mysqli_query($link2, 'SELECT @pcondisabled AS _test'))
		printf("[005] [%d] %s\n", mysqli_errno($link2), mysqli_error($link2));

	$row = mysqli_fetch_assoc($res);
	printf("Connecction 2 - SELECT @pcondisabled -> '%s'\n", $row['_test']);
	mysqli_free_result($res);

	if ($link1 === $link2)
		printf("[006] Links should not be identical\n");

	mysqli_close($link1);
	mysqli_close($link2);
	print "done!";
?>
--EXPECTF--
Warning: my_mysqli_connect(): Persistent connections are disabled. Downgrading to normal in %s on line %d

Warning: my_mysqli_connect(): Persistent connections are disabled. Downgrading to normal in %s on line %d
Connecction 1 - SELECT @pcondisabled -> 'Connection 1'
Connecction 2 - SELECT @pcondisabled -> ''
done!