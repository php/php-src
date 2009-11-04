--TEST--
Persistent connections and mysql.max_persistent
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifconnectfailure.inc');
	require_once('table.inc');

	if ($socket)
		$host = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$host = sprintf("%s:%s", $host, $port);

	// we need a second DB user to test for a possible flaw in the ext/mysql[i] code
	if (!$link = mysql_connect($host, $user, $passwd, true))
		die(sprintf("skip Cannot connect [%d] %s", mysql_errno(), mysql_error()));

	if (!mysql_select_db($db, $link))
		die(sprintf("skip [%d] %s", mysql_errno($link), mysql_error($link)));

	if (!$res = mysql_query('SHOW VARIABLES LIKE "old_passwords"', $link)) {
		die(sprintf("skip [%d] %s", mysql_errno($link), mysql_error($link)));
	}

	if (mysql_num_rows($res) != 1) {
		die(sprintf("skip Can't check if old_passwords = ON"));
	}

	$row = mysql_fetch_assoc($res);
	mysql_free_result($res);
	if ($row['Value'] == "ON")
		die(sprintf("skip Test will fail because old_passwords = ON. Hint: old passwords are insecure!"));

	if (!$res = mysql_query("SELECT CURRENT_USER() AS _user", $link))
		die(sprintf("skip [%d] %s", mysql_errno($link), mysql_error($link)));

	$row = mysql_fetch_assoc($res);
	mysql_free_result($res);
	$host = substr($row['_user'], strrpos($row['_user'], "@") + 1, strlen($row['_user']));

	mysql_query('DROP USER pcontest', $link);
	mysql_query(sprintf('DROP USER pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);
	if (!mysql_query(sprintf('CREATE USER pcontest@"%s" IDENTIFIED BY "pcontest"', mysql_real_escape_string($host, $link)), $link)) {
		printf("skip Cannot create second DB user [%d] %s", mysql_errno($link), mysql_error($link));
		mysql_close($link);
		die();
	}

	// we might be able to specify the host using CURRENT_USER(), but...
	if (!mysql_query(sprintf('GRANT SELECT ON TABLE %s.test TO pcontest@"%s"', $db, mysql_real_escape_string($host, $link)), $link)) {
		printf("skip Cannot GRANT SELECT to second DB user [%d] %s", mysql_errno($link), mysql_error($link));
		mysql_query(sprintf('REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);
		mysql_query(sprintf('DROP USER pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);
		mysql_close($link);
		die();
	}

	mysql_close($link);
?>
--INI--
mysql.max_links=3
mysql.max_persistent=2
mysql.allow_persistent=1
--FILE--
<?php
	require_once("connect.inc");
	require_once('table.inc');

	if ($socket)
		$host = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$host = sprintf("%s:%s", $host, $port);

	if (!$plink = mysql_pconnect($host, $user, $passwd))
		printf("[001] Cannot connect using the second DB user created during SKIPIF, [%d] %s\n",
			mysql_errno(), mysql_error());

	if (!mysql_select_db($db, $plink))
		printf("[002] [%d] %s\n", mysql_errno($plink), mysql_error($plink));

	ob_start();
	phpinfo();
	$phpinfo = strip_tags(ob_get_contents());
	ob_end_clean();
	$phpinfo = substr($phpinfo, strpos($phpinfo, 'MySQL Support => enabled'), 500);
	if (!preg_match('@Active Persistent Links\s+=>\s+(\d+)@ismU', $phpinfo, $matches))
		printf("[003] Cannot get # active persistent links from phpinfo()");
	$num_plinks = $matches[1];

	if (!$res = mysql_query('SELECT id, label FROM test WHERE id = 1', $plink))
		printf("[004] Cannot run query on persistent connection of second DB user, [%d] %s\n",
			mysql_errno($plink), mysql_error($plink));

	if (!$row = mysql_fetch_assoc($res))
		printf("[005] Cannot run fetch result, [%d] %s\n",
			mysql_errno($plink), mysql_error($plink));
	mysql_free_result($res);
	var_dump($row);

	// change the password for the second DB user and kill the persistent connection
	if (!$res = mysql_query("SELECT CURRENT_USER() AS _user", $link))
		printf("[006] [%d] %s", mysql_errno($link), mysql_error($link));

	$row = mysql_fetch_assoc($res);
	mysql_free_result($res);
	$host = substr($row['_user'], strrpos($row['_user'], "@") + 1, strlen($row['_user']));

	$sql = sprintf('SET PASSWORD FOR pcontest@"%s" = PASSWORD("newpass")', mysql_real_escape_string($host, $link));
	if (!mysql_query($sql, $link))
		printf("[007] Cannot change PW of second DB user, [%d] %s\n", mysql_errno($link), mysql_error($link));

	// persistent connections cannot be closed but only be killed
	$pthread_id = mysql_thread_id($plink);
	if (!mysql_query(sprintf('KILL %d', $pthread_id), $link))
		printf("[008] Cannot KILL persistent connection of second DB user, [%d] %s\n", mysql_errno($link), mysql_error($link));
	// give the server a second to really kill the thread
	sleep(1);

	if (!$res = mysql_query("SHOW FULL PROCESSLIST", $link))
		printf("[009] [%d] %s\n", mysql_errno($link), mysql_error($link));

	$running_threads = array();
	while ($row = mysql_fetch_assoc($res))
		$running_threads[$row['Id']] = $row;
	mysql_free_result($res);

	if (isset($running_threads[$pthread_id]))
		printf("[010] Persistent connection has not been killed\n");

	// we might get the old handle
	if ($plink = @mysql_pconnect($host, 'pcontest', 'pcontest'))
		printf("[011] Can connect using the old password, [%d] %s\n",
			mysql_errno(), mysql_error());

	ob_start();
	phpinfo();
	$phpinfo = strip_tags(ob_get_contents());
	ob_end_clean();
	$phpinfo = substr($phpinfo, strpos($phpinfo, 'MySQL Support => enabled'), 500);
	if (!preg_match('@Active Persistent Links\s+=>\s+(\d+)@ismU', $phpinfo, $matches))
		printf("[012] Cannot get # active persistent links from phpinfo()\n");

	$num_plinks_kill = $matches[1];
	if ($num_plinks_kill > $num_plinks)
		printf("[013] Statistics seems to be wrong, got %d active persistent links, expecting < %d links\n",
			$num_plinks_kill, $num_plinks);

	// The first connection has been closed, the last pconnect() was unable to connect -> no connection open
	// We must be able to connect because max_persistent limit has not been reached
	if (!$plink = mysql_pconnect($host, 'pcontest', 'newpass'))
		die(sprintf("[014] Cannot connect using the second DB, [%d] %s\n",
			mysql_errno(), mysql_error()));

	if (!mysql_select_db($db, $plink))
		printf("[015] [%d] %s\n", mysql_errno($plink), mysql_error($plink));

	if (!$res = mysql_query('SELECT id, label FROM test WHERE id = 1', $plink))
		printf("[016] Cannot run query on persistent connection of second DB user, [%d] %s\n",
			mysql_errno($plink), mysql_error($plink));

	if (!$row = mysql_fetch_assoc($res))
		printf("[017] Cannot run fetch result, [%d] %s\n",
			mysql_errno($plink), mysql_error($plink));
	mysql_free_result($res);
	var_dump($row);

	mysql_query(sprintf('REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);
	mysql_query(sprintf('DROP USER pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);
	mysql_close($link);
	print "done!";
?>
--CLEAN--
<?php
// connect + select_db
require_once("connect.inc");
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[c001] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
 	  $host, $myhost, $user, $db, $port, $socket);
}

if (!$res = mysql_query("SELECT CURRENT_USER() AS _user", $link))
	printf("[c002] [%d] %s", mysql_errno($link), mysql_error($link));

$row = mysql_fetch_assoc($res);
mysql_free_result($res);
$host = substr($row['_user'], strrpos($row['_user'], "@") + 1, strlen($row['_user']));

@mysql_query(sprintf('REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);
@mysql_query(sprintf('DROP USER pcontest@"%s"', mysql_real_escape_string($host, $link)), $link);

mysql_close($link);
?>
--EXPECTF--
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
array(2) {
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
  [%u|b%"label"]=>
  %unicode|string%(1) "a"
}
done!
