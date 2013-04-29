--TEST--
mysql_create_db()
--SKIPIF--
<?php
require_once('skipif.inc');
if (!function_exists('mysql_create_db'))
	die("Skip mysql_create_db() exists only in old versions of the libmysql.");
?>
--FILE--
<?php
include "connect.inc";

$link   = NULL;
$tmp    = null;

if (false !== ($tmp = mysql_create_db()))
	printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = mysql_create_db($link, $link, $link)))
	printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if ($link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[003] Can connect to the server using host=%s, user=%s, passwd=***non_empty, dbname=%s, port=%s, socket=%s\n",
		$host, $user . 'unknown_really', $db, $port, $socket);

if (!mysql_query("CREATE DATABASE mysqlcreatedb", $link))
	die(sprintf("[004] Cannot create database, aborting test, [%d] %s\n", mysql_errno($link), mysql_error($link)));

if (!mysql_query("DROP DATABASE mysqlcreatedb", $link))
	printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (true !== ($tmp = mysql_create_db("mysqlcreatedb", $link)))
	printf("[006] Expecting boolean/true, got %s/%s, [%d] %s\n", gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

if (false !== ($tmp = mysql_create_db("mysqlcreatedb", $link)))
	printf("[007] Expecting boolean/false, got %s/%s, [%d] %s\n", gettype($tmp), $tmp, mysql_errno($link), mysql_error($link));

if (!mysql_query("DROP DATABASE mysqlcreatedb", $link))
	printf("[008] [%d] %s\n", mysql_errno($link), mysql_error($link));

print "done!";
?>
--CLEAN--
<?php
require_once('connect.inc');
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[c001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

if (!mysql_query("DROP DATABASE IF EXISTS mysqlcreatedb", $link))
	printf("[c002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysql_close($link);
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d

Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
