--TEST--
mysql_[p]connect() - max_links/max_persistent
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once('table.inc');

mysql_query('DROP USER pcontest', $link);
if (!mysql_query('CREATE USER pcontest IDENTIFIED BY "pcontest"', $link)) {
	printf("skip Cannot create second DB user [%d] %s", mysql_errno($link), mysql_error($link));
	mysql_close($link);
	die();
}

// we might be able to specify the host using CURRENT_USER(), but...
if (!mysql_query(sprintf("GRANT SELECT ON TABLE %s.test TO pcontest@'%%'", $db), $link)) {
	printf("skip Cannot GRANT SELECT to second DB user [%d] %s", mysql_errno($link), mysql_error($link));
	mysql_query('REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest', $link);
	mysql_query('DROP USER pcontest', $link);
	mysql_close($link);
	die();
}
mysql_close($link);
?>
--INI--
mysql.max_links=2
mysql.allow_persistent=1
mysql.max_persistent=1
--FILE--
<?php
require_once('connect.inc');

function my_connect($offset, $host, $user, $passwd, $db, $port, $socket) {
	if ($socket)
		$host = sprintf("%s:%s", $host, $socket);
	else if ($port)
		$host = sprintf("%s:%s", $host, $port);


	$link = mysql_pconnect($host, $user, $passwd);
	if (!$link) {
		printf("[%03d] Cannot connect using host '%s', user '%s', password '****', [%d] %s\n",
			$offset, $host, $user, $passwd,
			mysql_errno(), mysql_error());
		return false;
	}

	if (!mysql_select_db($db, $link))
		return false;

	return $link;
}

$links = array();

// try to open 2 links
$links[0] = my_connect(10, $host, $user, $passwd, $db, $port, $socket);
$links[1] = my_connect(20, $host, 'pcontest', 'pcontest', $db, $port, $socket);
if (false !== $links[1])
	printf("[030] Last connection should not have been allowed!\n");

// free some links but let index 1 remain
unset($links[1]);
mysql_close($links[0]);
unset($links[0]);

// should be allowed -> only open connection
$links[0] = my_connect(40, $host, $user, $passwd, $db, $port, $socket);
var_dump($links);

mysql_query('REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest', $links[0]);
mysql_query('DROP USER pcontest', $links[0]);

mysql_close($links[0]);
print "done!\n";
?>
--CLEAN--
<?php
// connect + select_db
require_once("connect.inc");
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[c001] Cannot connect to the server using host=%s/%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
 	  $host, $myhost, $user, $db, $port, $socket);
}

@mysql_query('REVOKE ALL PRIVILEGES, GRANT OPTION FROM pcontest', $link);
@mysql_query('DROP USER pcontest', $link);

mysql_close($link);
?>
--EXPECTF--
Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Warning: mysql_pconnect(): Too many open persistent links (1) in %s on line %d
[020] Cannot connect using host '%s', user '%s', password '****', [0] 0

Deprecated: mysql_pconnect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
array(1) {
  [0]=>
  resource(%d) of type (mysql link persistent)
}
done!
