--TEST--
Persistent connections and mysqli.max_links
--SKIPIF--
<?php
	require_once('skipif.inc');
	require_once('skipifemb.inc');
	require_once('skipifconnectfailure.inc');
	require_once('table.inc');

	mysqli_query($link, 'DROP USER pcontest');
	mysqli_query($link, 'DROP USER pcontest@localhost');

	if (!mysqli_query($link, 'CREATE USER pcontest@"%" IDENTIFIED BY "pcontest"') ||
		!mysqli_query($link, 'CREATE USER pcontest@localhost IDENTIFIED BY "pcontest"')) {
		printf("skip Cannot create second DB user [%d] %s", mysqli_errno($link), mysqli_error($link));
		mysqli_close($link);
		die("skip CREATE USER failed");
	}

	// we might be able to specify the host using CURRENT_USER(), but...
	if (!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pcontest@'%%'", $db)) ||
		!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pcontest@'localhost'", $db))) {
		printf("skip Cannot GRANT SELECT to second DB user [%d] %s", mysqli_errno($link), mysqli_error($link));
		mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest');
		mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest@localhost');
		mysqli_query($link, 'DROP USER pcontest@localhost');
		mysqli_query($link, 'DROP USER pcontest');
		mysqli_close($link);
		die("skip GRANT failed");
	}

	if (!($link_pcontest = @my_mysqli_connect($host, 'pcontest', 'pcontest', $db, $port, $socket))) {
		mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest');
		mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest@localhost');
		mysqli_query($link, 'DROP USER pcontest@localhost');
		mysqli_query($link, 'DROP USER pcontest');
		mysqli_close($link);
		die("skip CONNECT using new user failed");
    }
	mysqli_close($link);
?>
--INI--
mysqli.allow_persistent=1
mysqli.max_persistent=2
mysqli.rollback_on_cached_plink=1
--FILE--
<?php
	require_once("connect.inc");
	require_once('table.inc');


	if (!mysqli_query($link, 'DROP USER pcontest') ||
		!mysqli_query($link, 'DROP USER pcontest@localhost') ||
		!mysqli_query($link, 'CREATE USER pcontest@"%" IDENTIFIED BY "pcontest"') ||
		!mysqli_query($link, 'CREATE USER pcontest@localhost IDENTIFIED BY "pcontest"') ||
		!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pcontest@'%%'", $db)) ||
		!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pcontest@'localhost'", $db))) {
		printf("[000] Init failed, [%d] %s\n",
			mysqli_errno($plink), mysqli_error($plink));
	}

	var_dump(mysqli_get_links_stats(1));

	echo "Before pconnect:";
	var_dump(mysqli_get_links_stats());

	if (!$plink = my_mysqli_connect('p:' . $host, 'pcontest', 'pcontest', $db, $port, $socket))
		printf("[001] Cannot connect using the second DB user created during SKIPIF, [%d] %s\n",
			mysqli_connect_errno(), mysqli_connect_error());

	echo "After pconnect:";
	var_dump(mysqli_get_links_stats());

	ob_start();
	phpinfo();
	$phpinfo = strip_tags(ob_get_contents());
	ob_end_clean();

	$phpinfo = substr($phpinfo, strpos($phpinfo, 'MysqlI Support => enabled'), 500);
	if (!preg_match('@Active Persistent Links\s+=>\s+(\d+)@ismU', $phpinfo, $matches))
		printf("[002] Cannot get # active persistent links from phpinfo()\n");
	$num_plinks = $matches[1];

	if (!$res = mysqli_query($plink, 'SELECT id, label FROM test WHERE id = 1'))
		printf("[003] Cannot run query on persistent connection of second DB user, [%d] %s\n",
			mysqli_errno($plink), mysqli_error($plink));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[004] Cannot run fetch result, [%d] %s\n",
			mysqli_errno($plink), mysqli_error($plink));
	mysqli_free_result($res);
	var_dump($row);

	// change the password for the second DB user and kill the persistent connection
	if ((!mysqli_query($link, 'SET PASSWORD FOR pcontest = "newpass"') &&
			!mysqli_query($link, 'SET PASSWORD FOR pcontest = PASSWORD("newpass")'))||
			!mysqli_query($link, 'FLUSH PRIVILEGES'))
		printf("[005] Cannot change PW of second DB user, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

        // change the password for the second DB user and kill the persistent connection
	if ((!mysqli_query($link, 'SET PASSWORD FOR pcontest@localhost = "newpass"') &&
		!mysqli_query($link, 'SET PASSWORD FOR pcontest@localhost = PASSWORD("newpass")')) ||
			!mysqli_query($link, 'FLUSH PRIVILEGES'))
		printf("[006] Cannot change PW of second DB user, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	// persistent connections cannot be closed but only be killed
	$pthread_id = mysqli_thread_id($plink);
	if (!mysqli_query($link, sprintf('KILL %d', $pthread_id)))
		printf("[007] Cannot KILL persistent connection of second DB user, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	// give the server a second to really kill the thread
	sleep(1);

	if (!$res = mysqli_query($link, "SHOW FULL PROCESSLIST"))
		printf("[008] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$running_threads = array();
	while ($row = mysqli_fetch_assoc($res))
		$running_threads[$row['Id']] = $row;
	mysqli_free_result($res);

	if (isset($running_threads[$pthread_id]))
		printf("[009] Persistent connection has not been killed\n");

	echo "Before second pconnect:";
	var_dump(mysqli_get_links_stats());

	// this fails and we have 0 (<= $num_plinks) connections
	if ($plink = @my_mysqli_connect('p:' . $host, 'pcontest', 'pcontest', $db, $port, $socket))
		printf("[010] Can connect using the old password, [%d] %s\n",
			mysqli_connect_errno($link), mysqli_connect_error($link));

	echo "After second pconnect:";
	var_dump(mysqli_get_links_stats());

	ob_start();
	phpinfo();
	$phpinfo = strip_tags(ob_get_contents());
	ob_end_clean();
	$phpinfo = substr($phpinfo, stripos($phpinfo, 'MysqlI Support => enabled'), 500);
	if (!preg_match('@Active Persistent Links\s+=>\s+(\d+)@ismU', $phpinfo, $matches))
		printf("[010] Cannot get # of active persistent links from phpinfo()\n");

	var_dump(mysqli_get_links_stats());

	$num_plinks_kill = $matches[1];
	$sstats = mysqli_get_links_stats();
	if ($sstats['active_plinks'] != $num_plinks_kill) {
		printf("[010.2] Num of active plinks differ %s %s\n", $sstats['active_plinks'], $num_plinks_kill);
	}
	if ($num_plinks_kill > $num_plinks)
		printf("[011] Expecting Active Persistent Links < %d, got %d\n", $num_plinks, $num_plinks_kill);

	if (!$plink = my_mysqli_connect('p:' . $host, 'pcontest', 'newpass', $db, $port, $socket))
		printf("[012] Cannot connect using the new password, [%d] %s\n",
			mysqli_connect_errno(), mysqli_connect_error());

	if (!$res = mysqli_query($plink, 'SELECT id, label FROM test WHERE id = 1'))
		printf("[013] Cannot run query on persistent connection of second DB user, [%d] %s\n",
			mysqli_errno($plink), mysqli_error($plink));

	if (!$row = mysqli_fetch_assoc($res))
		printf("[014] Cannot run fetch result, [%d] %s\n",
			mysqli_errno($plink), mysqli_error($plink));
	mysqli_free_result($res);
	var_dump($row);

	if ($plink2 = my_mysqli_connect('p:' . $host, 'pcontest', 'newpass', $db, $port, $socket)) {
		printf("[015] Can open more persistent connections than allowed, [%d] %s\n",
			mysqli_connect_errno(), mysqli_connect_error());
		var_dump(mysqli_get_links_stats());
	}

	ob_start();
	phpinfo();
	$phpinfo = strip_tags(ob_get_contents());
	ob_end_clean();
	$phpinfo = substr($phpinfo, stripos($phpinfo, 'MysqlI Support => enabled'), 500);
	if (!preg_match('@Active Persistent Links\s+=>\s+(\d+)@ismU', $phpinfo, $matches))
		printf("[016] Cannot get # of active persistent links from phpinfo()\n");

	$num_plinks = $matches[1];
	if ($num_plinks > (int)ini_get('mysqli.max_persistent'))
		printf("[017] mysqli.max_persistent=%d allows %d open connections!\n", ini_get('mysqli.max_persistent'),$num_plinks);

	mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest');
	mysqli_query($link, 'DROP USER pcontest');
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

mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest');
mysqli_query($link, 'REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest@localhost');
mysqli_query($link, 'DROP USER pcontest@localhost');
mysqli_query($link, 'DROP USER pcontest');

mysqli_close($link);
?>
--EXPECTF--
Warning: mysqli_get_links_stats(): no parameters expected in %s on line %d
NULL
Before pconnect:array(3) {
  ["total"]=>
  int(1)
  ["active_plinks"]=>
  int(0)
  ["cached_plinks"]=>
  int(0)
}
After pconnect:array(3) {
  ["total"]=>
  int(2)
  ["active_plinks"]=>
  int(1)
  ["cached_plinks"]=>
  int(0)
}
array(2) {
  ["id"]=>
  string(1) "1"
  ["label"]=>
  string(1) "a"
}
Before second pconnect:array(3) {
  ["total"]=>
  int(2)
  ["active_plinks"]=>
  int(1)
  ["cached_plinks"]=>
  int(0)
}
After second pconnect:array(3) {
  ["total"]=>
  int(1)
  ["active_plinks"]=>
  int(0)
  ["cached_plinks"]=>
  int(0)
}
array(3) {
  ["total"]=>
  int(1)
  ["active_plinks"]=>
  int(0)
  ["cached_plinks"]=>
  int(0)
}
array(2) {
  ["id"]=>
  string(1) "1"
  ["label"]=>
  string(1) "a"
}
[015] Can open more persistent connections than allowed, [0] 
array(3) {
  ["total"]=>
  int(3)
  ["active_plinks"]=>
  int(2)
  ["cached_plinks"]=>
  int(0)
}
done!
