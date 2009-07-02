<?PHP
require_once('connect.inc');

// connect + select_db
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[clean] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
 	  $host, $myhost, $user, $db, $port, $socket);
}

if (!mysql_query('DROP TABLE IF EXISTS test', $link)) {
	printf("[clean] Failed to drop test table: [%d] %s\n", mysql_errno($link), mysql_error($link));
}

mysql_close($link);
?>