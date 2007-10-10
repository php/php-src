--TEST--
mysql_[p]connect() - safe_mode
--SKIPIF--
<?php
require_once('skipif.inc');
$link = @mysql_connect("", "", "", true);
if ($link)
	die("skip Test cannot be run if annonymous connections are allowed");
?>
--INI--
sql.safe_mode=1
--FILE--
<?php
require_once('connect.inc');
if ($socket)
	$host = sprintf("%s:%s", $host, $socket);
else if ($port)
	$host = sprintf("%s:%s", $host, $port);

if ($link = mysql_connect($host, $user, $passwd, true)) {
	printf("[001] Safe mode not working properly?\n");
	mysql_close($link);
}

if ($link = mysql_pconnect($host, $user, $passwd)) {
	printf("[002] Safe mode not working properly?\n");
	mysql_close($link);
}
print "done!\n";
?>
--EXPECTF--
Notice: mysql_connect(): SQL safe mode in effect - ignoring host/user/password information in %s on line %d

Warning: mysql_connect(): Access denied for user '%s'@'%s' (using password: NO) in %s on line %d

Notice: mysql_pconnect(): SQL safe mode in effect - ignoring host/user/password information in %s on line %d

Warning: mysql_pconnect(): Access denied for user '%s'@'%s' (using password: NO) in %s on line %d
done!