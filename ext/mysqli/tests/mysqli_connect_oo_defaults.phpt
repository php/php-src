--TEST--
new mysqli()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if ($socket != "")
		/* mysqli.default_socket requires non-empty string */
		ini_set('mysqli.default_socket', 'socket');

	ini_set('mysqli.default_port', 9999);
	ini_set('mysqli.default_pw', 'password');
	ini_set('mysqli.default_user', 'user');
	ini_set('mysqli.default_host', 'host');

	mysqli_report(MYSQLI_REPORT_OFF);
	mysqli_report(MYSQLI_REPORT_STRICT);

	if ($socket != "") {
		ini_set('mysqli.default_socket', $socket);
		try {
			$mysqli = mysqli_init();
			$mysqli->real_connect($host, $user, $passwd, $db, $port);

			if (!$res = $mysqli->query("SELECT 'mysqli.default_socket' AS testing"))
				printf("[001] [%d] %s\n", $mysqli->errno, $mysqli->error);
			$tmp = $res->fetch_assoc();
			$res->free_result();

			if (!isset($tmp['testing']) || $tmp['testing'] != 'mysqli.default_socket') {
				printf("[002] mysqli.default_socket not properly set?\n");
				var_dump($tmp);
			}

			$mysqli->close();

		} catch (mysqli_sql_exception $e) {
			printf("%s\n", $e->getMessage());
			printf("[002] Usage of mysqli.default_socket failed\n");
		}
	}

	ini_set('mysqli.default_port', $port);
	try {
		$mysqli = mysqli_init();
		$mysqli->real_connect($host, $user, $passwd, $db);

		if (!$res = $mysqli->query("SELECT 'mysqli.default_port' AS testing"))
			printf("[003] [%d] %s\n", $mysqli->errno, $mysqli->error);
		var_dump($res->fetch_assoc());
		$res->free_result();

		$mysqli->close();

	} catch (mysqli_sql_exception $e) {
		printf("%s\n", $e->getMessage());
		printf("[004] Usage of mysqli.default_port failed\n");
	}

	ini_set('mysqli.default_pw', $passwd);
	try {
		$mysqli = mysqli_init();
		$mysqli->real_connect($host, $user);
		$mysqli->select_db($db);

		if (!$res = $mysqli->query("SELECT 'mysqli.default_pw' AS testing"))
			printf("[005] [%d] %s\n", $mysqli->errno, $mysqli->error);
		var_dump($res->fetch_assoc());
		$res->free_result();

		$mysqli->close();

	} catch (mysqli_sql_exception $e) {
		printf("%s\n", $e->getMessage());
		printf("[006] Usage of mysqli.default_pw failed\n");
	}

	ini_set('mysqli.default_user', $user);
	try {
		$mysqli = mysqli_init();
		$mysqli->real_connect($host);
		$mysqli->select_db($db);

		if (!$res = $mysqli->query("SELECT 'mysqli.default_user' AS testing"))
			printf("[007] [%d] %s\n", $mysqli->errno, $mysqli->error);
		var_dump($res->fetch_assoc());
		$res->free_result();

		$mysqli->close();

	} catch (mysqli_sql_exception $e) {
		printf("%s\n", $e->getMessage());
		printf("[008] Usage of mysqli.default_user failed\n");
	}

	ini_set('mysqli.default_host', $host);
	try {
		$mysqli = mysqli_init();
		$mysqli->real_connect();
		$mysqli->select_db($db);

		if (!$res = $mysqli->query("SELECT 1"))
			printf("[009] [%d] %s\n", $mysqli->errno, $mysqli->error);
		$res->free_result();

		if (!$res = $mysqli->query("SELECT SUBSTRING_INDEX(USER(),'@',1) AS username"))
			printf("[010] [%d] %s\n", $mysqli->errno, $mysqli->error);

		$tmp = $res->fetch_assoc();
		$res->free_result();
		if ($tmp['username'] !== $user)
			printf("[011] Expecting string/%s, got %s/%s\n", $user, gettype($tmp['username']), $tmp['username']);

		$mysqli->close();

	} catch (mysqli_sql_exception $e) {
		printf("%s\n", $e->getMessage());
		printf("[012] Usage of mysqli.default_host failed\n");
	}

	try {
		$link = mysqli_connect($host, $user, $passwd, null, ini_get('mysqli.default_port'));
		mysqli_select_db($link, $db);
		if (!$res = mysqli_query($link, "SELECT 'have been set' AS all_defaults"))
			printf("[013] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_assoc($res));
		mysqli_free_result($res);
		mysqli_close($link);
	} catch (mysqli_sql_exception $e) {
		printf("%s\n", $e->getMessage());
		printf("[014] Usage of mysqli_connect() has failed\n");
	}

	try {
		$link = mysqli_connect($host, $user, $passwd, null);
		mysqli_select_db($link, $db);
		if (!$res = mysqli_query($link, "SELECT 'have been set' AS all_defaults"))
			printf("[015] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
		var_dump(mysqli_fetch_assoc($res));
		mysqli_free_result($res);
		mysqli_close($link);
	} catch (mysqli_sql_exception $e) {
		printf("%s\n", $e->getMessage());
		printf("[016] Usage of mysqli_connect() has failed\n");
	}


	print "done!";
?>
--EXPECTF--
array(1) {
  ["testing"]=>
  string(19) "mysqli.default_port"
}
array(1) {
  ["testing"]=>
  string(17) "mysqli.default_pw"
}
array(1) {
  ["testing"]=>
  string(19) "mysqli.default_user"
}
array(1) {
  ["all_defaults"]=>
  string(13) "have been set"
}
array(1) {
  ["all_defaults"]=>
  string(13) "have been set"
}
done!
