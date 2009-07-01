--TEST--
mysql connect
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
require_once('connect.inc');
$test = '';

if ($socket)
	$host = sprintf("%s:%s", $host, $socket);
else if ($port)
	$host = sprintf("%s:%s", $host, $port);

/*** test mysql_connect localhost ***/
$db = mysql_connect($host, $user, $passwd);
$test .= ($db) ? '1' : '0';
mysql_close($db);

/*** test mysql_connect localhost:port ***/
$db = mysql_connect($host, $user, $passwd, true);
$test .= ($db) ? '1' : '0';
mysql_close($db);

var_dump($test);
print "done!";
?>
--EXPECTF--
%unicode|string%(2) "11"
done!
