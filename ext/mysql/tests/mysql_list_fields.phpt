--TEST--
mysql_list_fields()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once "connect.inc";

$tmp    = NULL;
$link   = NULL;

// This will implicitly try to connect, and we don't want it
//if (false !== ($tmp = mysql_list_fields($link, $link)))
//	printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

require('table.inc');

if (!$res = mysql_list_fields($db, 'test', $link))
	printf("[003] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (2 !== ($num = mysql_num_fields($res)))
	printf("[004] Expecting two fields, got %d. [%d] %s\n", $num, mysql_errno($link), mysql_error($link));

mysql_free_result($res);

if (!mysql_query("DROP TABLE IF EXISTS test2", $link) ||
	!mysql_query("CREATE TABLE test2(id INT)", $link))
	printf("[005] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (!$res = mysql_list_fields($db, 'test%', $link))
	printf("[006] [%d] %s\n", mysql_errno($link), mysql_error($link));

if (3 !== ($num = mysql_num_fields($res)))
	printf("[007] Expecting 3 fields from test and test2, got %d. [%d] %s\n", $num, mysql_errno($link), mysql_error($link));

mysql_free_result($res);
mysql_close($link);

print "done!";
?>
--CLEAN--
<?php
require_once('connect.inc');
if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[c001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

if (!mysql_query("DROP TABLE IF EXISTS test", $link))
	printf("[c002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

if (!mysql_query("DROP TABLE IF EXISTS test2", $link))
	printf("[c002] [%d] %s\n", mysqli_errno($link), mysqli_error($link));

mysql_close($link);
?>
--EXPECTF--
done!
