--TEST--
mysqli_connect()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	/* we need to check, if the server allows anonymous login (empty user) */
	$tmp = @mysqli_connect('localhost');
	$anon_allow = (gettype($tmp) == "object");

	$exptype = ($anon_allow) ? "mysqli_object" : "false";

	$obj = new stdClass();
	if (!is_null($tmp = @mysqli_connect($obj)))
		printf("[001] Expecting NULL got %s/%s\n", gettype($tmp), $tmp);

	$tmp = @mysqli_connect($link);
	if (($anon_allow && gettype($tmp) != "object") || (!$anon_allow && $tmp != false)) {
		printf("[002] Expecting %s, got %s/%s\n", $exptype, gettype($tmp), $tmp);
	}

	$tmp = @mysqli_connect($link, $link);
	if (($anon_allow && gettype($tmp) != "object") || (!$anon_allow && $tmp != false)) {
		printf("[003] Expecting %s, got %s/%s\n", $exptype, gettype($tmp), $tmp);
	}

	$tmp = @mysqli_connect($link, $link, $link);
	if (($anon_allow && gettype($tmp) != "object") || (!$anon_allow && $tmp != false)) {
		printf("[004] Expecting %s, got %s/%s\n", $exptype, gettype($tmp), $tmp);
	}

	$tmp = @mysqli_connect($link, $link, $link, $link);
	if (($anon_allow && gettype($tmp) != "object") || (!$anon_allow && $tmp != false)) {
		printf("[005] Expecting %s, got %s/%s\n", $exptype, gettype($tmp), $tmp);
	}

	$tmp = @mysqli_connect($link, $link, $link, $link, $link);
	if (($anon_allow && gettype($tmp) != "object") || (!$anon_allow && $tmp != false)) {
		printf("[006] Expecting %s, got %s/%s\n", $exptype, gettype($tmp), $tmp);
	}

	$tmp = @mysqli_connect($link, $link, $link, $link, $link, $link);
	if (($anon_allow && gettype($tmp) != "object") || (!$anon_allow && $tmp != false)) {
		printf("[007] Expecting %s, got %s/%s\n", $exptype, gettype($tmp), $tmp);
	}

	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[008] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	mysqli_close($link);

	if ($link = mysqli_connect($host, $user . 'unknown_really', $passwd . 'non_empty', $db, $port, $socket))
		printf("[009] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
			$host, $user . 'unknown_really', $db, $port, $socket);

	if (false !== $link)
		printf("[010] Expecting boolean/false, got %s/%s\n", gettype($link), $link);

	// Run the following tests without an anoynmous MySQL user and use a password for the test user!
	ini_set('mysqli.default_socket', $socket);
	if (!is_object($link = mysqli_connect($host, $user, $passwd, $db, $port))) {
		printf("[011] Usage of mysqli.default_socket failed\n") ;
	} else {
		if (!$res = mysqli_query($link, "SELECT 'mysqli.default_socket' AS 'testing'"))
			printf("[012] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_assoc($res));
		mysqli_free_result($res);
		mysqli_close($link);
	}

	ini_set('mysqli.default_port', $port);
	if (!is_object($link = mysqli_connect($host, $user, $passwd, $db))) {
		printf("[013] Usage of mysqli.default_port failed\n") ;
	} else {
		if (!$res = mysqli_query($link, "SELECT 'mysqli.default_port' AS 'testing'"))
			printf("[014] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_assoc($res));
		mysqli_free_result($res);
		mysqli_close($link);
	}

	ini_set('mysqli.default_pw', $passwd);
	if (!is_object($link = mysqli_connect($host, $user))) {
		printf("[015] Usage of mysqli.default_pw failed\n") ;
	} else {
		if (!$res = mysqli_query($link, "SELECT 'mysqli.default_pw' AS 'testing'"))
			printf("[016] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_assoc($res));
		mysqli_free_result($res);
		mysqli_close($link);
	}

	ini_set('mysqli.default_user', $user);
	if (!is_object($link = mysqli_connect($host))) {
		printf("[017] Usage of mysqli.default_user failed\n") ;
	} else {
		if (!$res = mysqli_query($link, "SELECT 'mysqli.default_user' AS 'testing'"))
			printf("[018] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_array($res, MYSQLI_BOTH));
		mysqli_free_result($res);
		mysqli_close($link);
	}

	ini_set('mysqli.default_host', $host);
	if (!is_object($link = mysqli_connect())) {
		printf("[019] Usage of mysqli.default_host failed\n") ;
	} else {
		if (!$res = mysqli_query($link, "SELECT 'mysqli.default_host' AS 'testing'"))
			printf("[020] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_array($res, MYSQLI_NUM));
		mysqli_free_result($res);
		mysqli_close($link);
	}

	if ($IS_MYSQLND) {
		ini_set('mysqli.default_host', 'p:' . $host);
			if (!is_object($link = mysqli_connect())) {
				printf("[021] Usage of mysqli.default_host (persistent) failed\n") ;
		} else {
			if (!$res = mysqli_query($link, "SELECT 'mysqli.default_host (persistent)' AS 'testing'"))
				printf("[022] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
			$tmp = mysqli_fetch_assoc($res);
			if ($tmp['testing'] !== 'mysqli.default_host (persistent)') {
				printf("[023] Result looks strange - check manually, [%d] %s\n",
					mysqli_errno($link), mysqli_error($link));
				var_dump($tmp);
			}
			mysqli_free_result($res);
			mysqli_close($link);
		}

		ini_set('mysqli.default_host', 'p:');
		if (is_object($link = @mysqli_connect())) {
			printf("[024] Usage of mysqli.default_host=p: did not fail\n") ;
			mysqli_close($link);
		}
	}

	print "done!";
?>
--EXPECTF--
Warning: mysqli_connect(): (%d/%d): Access denied for user '%s'@'%s' (using password: YES) in %s on line %d
array(1) {
  [%u|b%"testing"]=>
  %unicode|string%(21) "mysqli.default_socket"
}
array(1) {
  [%u|b%"testing"]=>
  %unicode|string%(19) "mysqli.default_port"
}
array(1) {
  [%u|b%"testing"]=>
  %unicode|string%(17) "mysqli.default_pw"
}
array(2) {
  [0]=>
  %unicode|string%(19) "mysqli.default_user"
  [%u|b%"testing"]=>
  %unicode|string%(19) "mysqli.default_user"
}
array(1) {
  [0]=>
  %unicode|string%(19) "mysqli.default_host"
}
done!