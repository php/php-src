--TEST--
mysql_pconnect() - disabling feature
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysql.allow_persistent=0
mysql.max_persistent=1
mysql.max_links=2
--FILE--
<?php
	require_once("connect.inc");
	require_once("table.inc");

	if (($plink = my_mysql_connect($host, $user, $passwd, $db, $port, $socket, NULL, true)))
		printf("[001] Can connect to the server.\n");

	if (($res = mysql_query('SELECT id FROM test ORDER BY id ASC', $plink)) &&
			($row = mysql_fetch_assoc($res)) &&
			(mysql_free_result($res))) {
		printf("[002] Can fetch data using persistent connection! Data = '%s'\n",
			$row['id']);
	}

	$thread_id = mysql_thread_id($plink);
	mysql_close($plink);

	if (!($plink = my_mysql_connect($host, $user, $passwd, $db, $port, $socket, NULL, true)))
		printf("[003] Cannot connect, [%d] %s\n", mysql_errno(), mysql_error());

	if (mysql_thread_id($plink) != $thread_id)
		printf("[004] Looks like the second call to pconnect() did not give us the same connection.\n");

	$thread_id = mysql_thread_id($plink);
	mysql_close($plink);

	if (!($plink = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)))
		printf("[005] Cannot connect, [%d] %s\n", mysql_errno(), mysql_error());

	if (mysql_thread_id($plink) == $thread_id)
		printf("[006] Looks like connect() did not return a new connection.\n");

	print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
[001] Can connect to the server.
[002] Can fetch data using persistent connection! Data = '1'
done!