--TEST--
mysqli_real_connect()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (NULL !== ($tmp = @mysqli_real_connect($link)))
		printf("[001a] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_real_connect($link, $link)))
		printf("[001b] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_real_connect($link, $link, $link)))
		printf("[001c] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_real_connect($link, $link, $link, $link)))
		printf("[001d] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_real_connect($link, $link, $link, $link, $link)))
		printf("[001e] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_real_connect($link, $link, $link, $link, $link, $link)))
		printf("[001f] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_real_connect($link, $link, $link, $link, $link, $link, $link)))
		printf("[001g] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	//  ( mysqli link [, string hostname [, string username [, string passwd [, string dbname [, int port [, string socket [, int flags]]]]]]]
	if (NULL !== ($tmp = @mysqli_real_connect($link, $link, $link, $link, $link, $link, $link, $link)))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = mysqli_init())
		printf("[002] mysqli_init() failed\n");

	if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket))
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	mysqli_close($link);
	if (!$link = mysqli_init())
		printf("[004] mysqli_init() failed\n");

	if (false !== ($tmp = mysqli_real_connect($link, $host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket)))
		printf("[005] Expecting boolean/false got %s/%s. Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n", gettype($tmp), $tmp, $host, $user . 'unknown_really', $db, $port, $socket);

	// Run the following tests without an anoynmous MySQL user and use a password for the test user!
	ini_set('mysqli.default_socket', $socket);
	if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port)) {
		printf("[006] Usage of mysqli.default_socket failed\n");
	} else {
		mysqli_close($link);
		if (!$link = mysqli_init())
			printf("[007] mysqli_init() failed\n");
	}

	ini_set('mysqli.default_port', $port);
	if (!mysqli_real_connect($link, $host, $user, $passwd, $db)) {
		printf("[008] Usage of mysqli.default_port failed\n");
	} else {
		mysqli_close($link);
		if (!$link = mysqli_init())
			printf("[009] mysqli_init() failed\n");
	}

	ini_set('mysqli.default_pw', $passwd);
	if (!mysqli_real_connect($link, $host, $user)) {
		printf("[010] Usage of mysqli.default_pw failed\n") ;
	} else {
		mysqli_close($link);
		if (!$link = mysqli_init())
			printf("[011] mysqli_init() failed\n");
	}

	ini_set('mysqli.default_user', $user);
	if (!mysqli_real_connect($link, $host)) {
		printf("[012] Usage of mysqli.default_user failed\n") ;
	} else {
		mysqli_close($link);
		if (!$link = mysqli_init())
			printf("[011] mysqli_init() failed\n");
	}

	ini_set('mysqli.default_host', $host);
	if (!mysqli_real_connect($link)) {
		printf("[014] Usage of mysqli.default_host failed\n") ;
	} else {
		mysqli_close($link);
		if (!$link = mysqli_init())
			printf("[015] mysqli_init() failed\n");
	}

	// CLIENT_MULTI_STATEMENTS - should be disabled silently
	if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, 65536))
		printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if ($res = mysqli_query($link, "SELECT 1 AS a; SELECT 2 AS b")) {
		printf("[017] Should have failed. CLIENT_MULTI_STATEMENT should have been disabled.\n");
		var_dump($res->num_rows);
		mysqli_next_result($link);
		$res = mysqli_store_result($link);
		var_dump($res->num_rows);
	}


	mysqli_close($link);
	if (!$link = mysqli_init())
		printf("[018] mysqli_init() failed\n");

	if (ini_get('open_basedir')) {
		// CLIENT_LOCAL_FILES should be blocked - but how to test it ?!

		if (!mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket, 128))
			printf("[019] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

		$filename = sys_get_temp_dir() . DIRECTORY_SEPARATOR . 'mysqli_real_connect_phpt';
		if (!$fp = fopen($filename, 'w'))
			printf("[020] Cannot open temporary file %s\n", $filename);

		fwrite($fp, '100;z');
		fclose($fp);

		// how do we test if gets forbidden because of a missing right or the flag, this test is partly bogus ?
		if (mysqli_query($link, "LOAD DATA LOCAL INFILE '$filename' INTO TABLE test FIELDS TERMINATED BY ';'"))
			printf("[021] LOAD DATA INFILE should have been forbidden!\n");

		unlink($filename);
	}

	mysqli_close($link);
	@var_dump($link);

	if ($IS_MYSQLND) {
		ini_set('mysqli.default_host', 'p:' . $host);
		$link = mysqli_init();
		if (!@mysqli_real_connect($link)) {
			printf("[022] Usage of mysqli.default_host=p:%s (persistent) failed\n", $host) ;
		} else {
			if (!$res = mysqli_query($link, "SELECT 'mysqli.default_host (persistent)' AS 'testing'"))
				printf("[023] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
			$tmp = mysqli_fetch_assoc($res);
			if ($tmp['testing'] !== 'mysqli.default_host (persistent)') {
				printf("[024] Result looks strange - check manually, [%d] %s\n",
					mysqli_errno($link), mysqli_error($link));
				var_dump($tmp);
			}
			mysqli_free_result($res);
			mysqli_close($link);
		}

		ini_set('mysqli.default_host', 'p:');
		$link = mysqli_init();
		if (@mysqli_real_connect($link)) {
			printf("[025] Usage of mysqli.default_host=p: did not fail\n") ;
			mysqli_close($link);
		}
		@mysqli_close($link);
	}

	if (NULL !== ($tmp = mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)))
		printf("[026] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
Warning: mysqli_real_connect(): (%s/%d): Access denied for user '%s'@'%s' (using password: YES) in %s on line %d
object(mysqli)#%d (%d) {
  [%u|b%"affected_rows"]=>
  NULL
  [%u|b%"client_info"]=>
  %s
  [%u|b%"client_version"]=>
  int(%d)
  [%u|b%"connect_errno"]=>
  int(%d)
  [%u|b%"connect_error"]=>
  NULL
  [%u|b%"errno"]=>
  %s
  [%u|b%"error"]=>
  %s
  [%u|b%"error_list"]=>
  NULL
  [%u|b%"field_count"]=>
  NULL
  [%u|b%"host_info"]=>
  NULL
  [%u|b%"info"]=>
  NULL
  [%u|b%"insert_id"]=>
  NULL
  [%u|b%"server_info"]=>
  NULL
  [%u|b%"server_version"]=>
  NULL
  [%u|b%"stat"]=>
  NULL
  [%u|b%"sqlstate"]=>
  NULL
  [%u|b%"protocol_version"]=>
  NULL
  [%u|b%"thread_id"]=>
  NULL
  [%u|b%"warning_count"]=>
  NULL
}

Warning: mysqli_real_connect(): Couldn't fetch mysqli in %s on line %d
done!
