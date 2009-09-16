--TEST--
mysqlnd.net_read_timeout > default_socket_timeout
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('connect.inc');
?>
--INI--
default_socket_timeout=1
mysqlnd.net_read_timeout=12
max_execution_time=12
--FILE--
<?php
	set_time_limit(12);
	include ("connect.inc");

	if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Connect failed, [%d] %s\n", mysql_errno(), mysqlerror());
	}

	if (!$res = mysql_query("SELECT SLEEP(6)", $link))
		printf("[002] [%d] %s\n",  mysql_errno($link), mysql_error($link));

	var_dump(mysql_fetch_assoc($res));

	mysql_free_result($res);
	mysql_close($link);

	print "done!";
?>
--EXPECTF--
array(1) {
  [%u|b%"SLEEP(6)"]=>
  %unicode|string%(1) "0"
}
done!