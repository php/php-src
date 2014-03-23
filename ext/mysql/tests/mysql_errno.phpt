--TEST--
mysql_errno()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$tmp    = NULL;
$link   = NULL;

if (false !== ($tmp = @mysql_errno()))
	printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (null !== ($tmp = @mysql_errno($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!is_null($tmp = @mysql_errno($link, 'too many args')))
	printf("[002b] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
}
var_dump(mysql_errno($link));

if (!mysql_query('DROP TABLE IF EXISTS test', $link)) {
	printf("[004] Failed to drop old test table: [%d] %s\n", mysql_errno($link), mysql_errno($link));
}

mysql_query('SELECT * FROM test', $link);
var_dump(mysql_errno($link));

mysql_close($link);

var_dump(mysql_errno($link));

if ($link = @mysql_connect($host . '_unknown', $user . '_unknown', $passwd, true)) {
	printf("[005] Can connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host . '_unknown', $user . '_unknown', $db, $port, $socket);
} else {
	$errno = mysql_errno();
	if (!is_int($errno))
		printf("[006] Expecting int/any (e.g 1046, 2005) got %s/%s\n", gettype($errno), $errno);

}

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
int(0)
int(%d)

Warning: mysql_errno(): supplied resource is not a valid MySQL-Link resource in %s on line %d
bool(false)
done!
