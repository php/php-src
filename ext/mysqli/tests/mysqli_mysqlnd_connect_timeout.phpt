--TEST--
MYSQLI_OPT_CONNECT_TIMEOUT check
--SKIPIF--
<?php
require_once('connect.inc');
if (!$IS_MYSQLND)
    die("SKIP mysqlnd only test");
?>
--INI--
default_socket_timeout=60
max_execution_time=60
mysqlnd.net_read_timeout=60
--FILE--
<?php
	putenv( 'MYSQL_TEST_HOST=127.0.0.1' );
	putenv( 'MYSQL_TEST_PORT=58379' );
    include ("connect.inc");

	$sock = socket_create(AF_INET, SOCK_STREAM, 0);
	if (!socket_bind($sock, '127.0.0.1', 58379)) {
		die("SKIP IPv4/port 58379 not available");
	}
	socket_listen($sock, 0);
	pcntl_alarm( 5 );

	$link = mysqli_init();
	mysqli_options($link, MYSQLI_OPT_CONNECT_TIMEOUT, 1);

	if (!$link = @mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, 0)) {
		printf("[001] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}

	if (is_resource($link)) {
		mysqli_close($link);
	}

	print "done!";
?>
--EXPECT--
[001] Connect failed, [2006] MySQL server has gone away
done!