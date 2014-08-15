--TEST--
mysql_client_encoding()
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

if (!$link = my_mysql_connect($host, $user, $passwd, $db, $port, $socket))
	printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket);

$default_link_enc = mysql_client_encoding();
$link_enc = mysql_client_encoding($link);

if ($default_link_enc !== $link_enc)
	printf("[003] %s != %s, [%d] %s\n", $default_link_enc, $link_enc, mysql_errno($link), mysql_error($link));

if (!$res = mysql_query('SELECT version() AS server_version', $link))
	printf("[004] [%d] %s\n", mysql_errno($link), mysql_error($link));
$tmp = mysql_fetch_assoc($res);
mysql_free_result($res);
$version = explode('.', $tmp['server_version']);
if (empty($version))
	printf("[005] Cannot determine server version, need MySQL Server 4.1+ for the test!\n");

if ($version[0] <= 4 && $version[1] < 1)
	printf("[006] Need MySQL Server 4.1+ for the test!\n");

if (!$res = mysql_query('SELECT @@character_set_connection AS charset, @@collation_connection AS collation', $link))
	printf("[007] [%d] %s\n", mysql_errno($link), mysql_error($link));
$tmp = mysql_fetch_assoc($res);
mysql_free_result($res);
if (!$tmp['charset'])
	printf("[008] Cannot determine current character set and collation\n");

if ($link_enc !== $tmp['charset']) {
	if ($link_enc === $tmp['collation']) {
		printf("[009] Known bug, collation instead of chatset returned, http://bugs.mysql.com/bug.php?id=7923\n");
	} else {
		printf("[009] Check manually, watch out for unicode and others\n");
		var_dump($link_enc);
		var_dump($tmp);
	}
}

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) &&
    (version_compare(PHP_VERSION, '6.9.9', '<=') == 1) &&
    function_exists('is_unicode')) {
// unicode mode
	if (!is_unicode($default_link_enc) || !is_unicode($link_enc)) {
		printf("[010] No unicode returned!\n");
		var_dump($default_link_enc);
		var_dump($link_enc);
	}
}

mysql_close($link);

if (false !== ($tmp = @mysql_client_encoding($link)))
	printf("[012] Expecting boolean/false, got %s/%s\n", gettype($tmp), $tmp);

print "done!";
?>
--EXPECTF--
Deprecated: mysql_connect(): The mysql extension is deprecated and will be removed in the future: use mysqli or PDO instead in %s on line %d
done!
