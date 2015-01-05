--TEST--
mysql_pconnect() - killing persitent connection
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysql.allow_persistent=1
mysql.max_persistent=2
--FILE--
<?php
	include "connect.inc";
	include "table.inc";

	if ($socket)
		$myhost = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$myhost = sprintf("%s:%s", $host, $port);
	else
		$myhost = $host;

	if (!($plink = mysql_pconnect($myhost, $user, $passwd)))
		printf("[001] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $myhost, $user, $db, $port, $socket);
	mysql_select_db($db, $plink);

	$pthread_id = mysql_thread_id($plink);
	$thread_id = mysql_thread_id($link);

	if (!($res = mysql_query("SHOW FULL PROCESSLIST", $link)))
		printf("[002] Cannot get processlist, [%d] %s\n", mysql_errno($link), mysql_error($link));

	$processlist = array();
	while ($row = mysql_fetch_assoc($res))
		$processlist[$row['Id']] = $row;
	mysql_free_result($res);

	if (!isset($processlist[$thread_id]))
		printf("[003] Cannot find regular connection thread in process list, [%d] %s\n", mysql_errno($link), mysql_error($link));
	if (!isset($processlist[$pthread_id]))
		printf("[004] Cannot find persistent connection thread in process list, [%d] %s\n", mysql_errno($link), mysql_error($link));

	if (!mysql_query(sprintf("KILL %d", $pthread_id), $link))
		printf("[005] Cannot kill persistent connection thread, [%d] %s\n", mysql_errno($link), mysql_error($link));

	while (1) {
		if (!($res = mysql_query("SHOW FULL PROCESSLIST", $link)))
			printf("[006] Cannot get processlist, [%d] %s\n", mysql_errno($link), mysql_error($link));

		$processlist2 = array();
		while ($row = mysql_fetch_assoc($res))
			$processlist2[$row['Id']] = $row;
		mysql_free_result($res);
		if (isset($processlist2[$pthread_id])) {
			sleep(1);
		} else {
			break;
		}
	}

	if (!isset($processlist2[$thread_id]))
		printf("[007] Cannot find regular connection thread in process list, [%d] %s\n", mysql_errno($link), mysql_error($link));

	mysql_close($plink);

	if (!($plink = @mysql_pconnect($myhost, $user, $passwd)))
		printf("[009] Cannot create new persistent connection, [%d] %s\n", mysql_errno(), mysql_error());
	mysql_select_db($db, $plink);

	if (!($res = mysql_query("SELECT 1", $plink)))
		printf("[010] Cannot run query on new persistent connection, [%d] %s\n", @mysql_errno($plink), @mysql_error($plink));
	mysql_free_result($res);

	var_dump(mysql_ping($plink));

	if (!($res = mysql_query("SELECT 1", $plink)))
		printf("[011] Cannot run query on new persistent connection, [%d] %s\n", @mysql_errno($plink), @mysql_error($plink));
	mysql_free_result($res);

	if (!($link2 = mysql_connect($myhost, $user, $passwd, true)))
		printf("[012] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $myhost, $user, $db, $port, $socket);
	mysql_select_db($db, $link2);
	if (!mysql_query(sprintf("KILL %d", $thread_id), $link2))
		printf("[013] Cannot kill regular connection thread, [%d] %s\n", mysql_errno($link2), mysql_error($link2));

	if (!($link = mysql_connect($myhost, $user, $passwd, true)))
		printf("[014] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $myhost, $user, $db, $port, $socket);
	mysql_select_db($db, $link);
	if (!($res = mysql_query("SELECT * FROM test", $link)))
		printf("[015] Cannot run query on new regular connection, [%d] %s\n", @mysql_errno($link), @mysql_error($link));

	if (!($res = mysql_query("SELECT * FROM test", $link2)))
		printf("[016] Cannot run query on other regular connection, [%d] %s\n", @mysql_errno($link2), @mysql_error($link2));

	mysql_free_result($res);
	mysql_close($plink);
	mysql_close($link);
	mysql_close($link2);
	print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
bool(true)

Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
