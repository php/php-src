--TEST--
mysql_close()
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

if (false !== ($tmp = @mysql_close()))
	printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

if (NULL !== ($tmp = @mysql_close($link, $link)))
	printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

$tmp = @mysql_close(NULL);
if (null !== $tmp)
	printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

$tmp = mysql_close($link);
if (true !== $tmp)
	printf("[005] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

if (false !== ($tmp = @mysql_query("SELECT 1", $link)))
	printf("[006] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!\n";
?>
--EXPECTF--
done!
