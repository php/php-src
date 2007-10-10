--TEST--
mysql_pconnect()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--INI--
mysql.max_persistent=10
mysql.allow_persistent=1
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	// mysql_pconnect ( [string server [, string username [, string password [, bool new_link [, int client_flags]]]]] )
	if (NULL !== ($tmp = @mysql_pconnect($link, $link, $link, $link, $link, $link)))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	$myhost = (is_null($socket)) ? ((is_null($port)) ? $host : $host . ':' . $port) : $host . ':' . $socket;
	if (!$link = mysql_pconnect($myhost, $user, $passwd, true))
		printf("[002] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $myhost, $user, $db, $port, $socket);

	mysql_close($link);

	if ($link = mysql_pconnect($myhost, $user . 'unknown_really', $passwd . 'non_empty', true))
		printf("[003] Can connect to the server using host=%s/%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
			$host, $myhost, $user . 'unknown_really', $db, $port, $socket);

	if (false !== $link)
		printf("[004] Expecting boolean/false, got %s/%s\n", gettype($link), $link);

	// Run the following tests without an anoynmous MySQL user and use a password for the test user!


	if (!ini_get('sql.safe_mode')) {

		if ($socket) {
			ini_set('mysql.default_socket', $socket);
			if (!is_resource($link = mysql_pconnect($host, $user, $passwd, true))) {
				printf("[005] Usage of mysql.default_socket failed\n") ;
			} else {
				mysql_close($link);
			}
		} else {
			ini_set('mysql.default_socket', null);
		}

		ini_set('mysql.default_port', $port);
		if (!is_resource($link = mysql_pconnect($host, $user, $passwd, true))) {
			printf("[006] Usage of mysql.default_port failed\n") ;
		} else {
			mysql_close($link);
		}

		ini_set('mysql.default_password', $passwd);
		if (!is_resource($link = mysql_pconnect($myhost, $user))) {
			printf("[007] Usage of mysql.default_password failed\n") ;
		} else {
			mysql_close($link);
		}

		ini_set('mysql.default_user', $user);
		if (!is_resource($link = mysql_pconnect($myhost))) {
			printf("[008] Usage of mysql.default_user failed\n");
		} else {
			mysql_close($link);
		}

		ini_set('mysql.default_host', $myhost);
		if (!is_resource($link = mysql_pconnect())) {
			printf("[009] Usage of mysql.default_host failed\n") ;
		} else {
			mysql_close($link);
		}
	}

	print "done!";
?>
--EXPECTF--
Warning: mysql_pconnect(): Access denied for user '%s'@'%s' (using password: YES) in %s on line %d
done!
