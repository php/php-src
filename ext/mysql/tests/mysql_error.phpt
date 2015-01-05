--TEST--
mysql_error()
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

if (false !== ($tmp = @mysql_error()))
	printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_error($link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!is_null($tmp = @mysql_error($link, 'too many args')))
	printf("[002b] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket)) {
	printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);
}

$tmp = mysql_error($link);
if (!is_string($tmp) || ('' !== $tmp))
	printf("[004] Expecting string/empty, got %s/%s. [%d] %s\n", gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

if (!mysql_query('DROP TABLE IF EXISTS test', $link)) {
	printf("[005] Failed to drop old test table: [%d] %s\n", mysql_errno($link), mysql_error($link));
}

mysql_query('SELECT * FROM test', $link);
$tmp = mysql_error($link);
if (!is_string($tmp) || !preg_match("/Table '\w*\.test' doesn't exist/su", $tmp))
	printf("[006] Expecting string/[Table... doesn't exit], got %s/%s. [%d] %s\n", gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) &&
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    !is_unicode($tmp)) {
	printf("[007] Expecting Unicode error message!\n");
	var_inspect($tmp);
}

mysql_close($link);

var_dump(mysql_error($link));

if ($link = @mysql_connect($host . '_unknown', $user . '_unknown', $passwd, true)) {
	printf("[008] Can connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host . '_unknown', $user . '_unknown', $db, $port, $socket);
}
if ('' == mysql_error())
	printf("[009] Connect error should have been set\n");

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Warning: mysql_error(): supplied resource is not a valid MySQL-Link resource in %s on line %d
bool(false)
done!
