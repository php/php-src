<?php
if (!function_exists('sys_get_temp_dir')) {
	function sys_get_temp_dir() {

		if (!empty($_ENV['TMP']))
			return realpath( $_ENV['TMP'] );
		if (!empty($_ENV['TMPDIR']))
			return realpath( $_ENV['TMPDIR'] );
		if (!empty($_ENV['TEMP']))
			return realpath( $_ENV['TEMP'] );

		$temp_file = tempnam(md5(uniqid(rand(), TRUE)), '');
		if ($temp_file) {
                	$temp_dir = realpath(dirname($temp_file));
			unlink($temp_file);
			return $temp_dir;
		}
		return FALSE;
	}
}

/* wrapper to simplify test porting */
function my_mysql_connect($host, $user, $passwd, $db, $port, $socket) {

	if ($socket)
		$host = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$host = sprintf("%s:%s", $host, $port);

	if (!$link = mysql_connect($host, $user, $passwd, true)) {
		printf("[000-a] Cannot connect using host '%s', user '%s', password '****', [%d] %s\n",
			$host, $user, $passwd,
			mysql_errno(), mysql_error());
		return false;
	}

	if (!mysql_select_db($db, $link)) {
		printf("[000-b] [%d] %s\n", mysql_errno($link), mysql_error($link));
		return false;
	}

	return $link;
}

/*
Default values are "localhost", "root", database "test" and empty password.
Change the MYSQL_TEST_* environment values if you want to use another configuration.
*/

$host		= getenv("MYSQL_TEST_HOST")	? getenv("MYSQL_TEST_HOST")	: "localhost";
$port		= getenv("MYSQL_TEST_PORT")	? getenv("MYSQL_TEST_PORT")	: 3306;
$user		= getenv("MYSQL_TEST_USER")	? getenv("MYSQL_TEST_USER")	: "root";
$passwd	= getenv("MYSQL_TEST_PASSWD")	? getenv("MYSQL_TEST_PASSWD")	: "";
$db			= getenv("MYSQL_TEST_DB")	? getenv("MYSQL_TEST_DB")	: "test";
$engine	= getenv("MYSQL_TEST_ENGINE")	? getenv("MYSQL_TEST_ENGINE")	: "MyISAM";
$socket	= getenv("MYSQL_TEST_SOCKET")	? getenv("MYSQL_TEST_SOCKET")	: null;
$skip_on_connect_failure  = getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") ? getenv("MYSQL_TEST_SKIP_CONNECT_FAILURE") : true;

/* Development setting: test experimal features and/or feature requests that never worked before? */
$TEST_EXPERIMENTAL = (in_array(getenv("MYSQL_TEST_EXPERIMENTAL"), array(0, 1))) ?
	((1 == getenv("MYSQL_TEST_EXPERIMENTAL")) ? true : false) :
	false;

$IS_MYSQLND = stristr(mysql_get_client_info(), "mysqlnd");
?>
