--TEST--
Bug #55283 (SSL options set by mysqli_ssl_set ignored for MySQLi persistent connections)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
require_once("connect.inc");

if ($IS_MYSQLND && !extension_loaded("openssl"))
	die("skip PHP streams lack support for SSL. mysqli is compiled to use mysqlnd which uses PHP streams in turn.");

if (!($link = @my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)))
	die(sprintf("skip Connect failed, [%d] %s", mysqli_connect_errno(), mysqli_connect_error()));

$row = NULL;
if ($res = $link->query('SHOW VARIABLES LIKE "have_ssl"')) {
	$row = $res->fetch_row();
} else {
	if ($link->errno == 1064 && ($res = $link->query("SHOW VARIABLES"))) {
		while ($row = $res->fetch_row())
			if ($row[0] == 'have_ssl')
				break;
	} else {
		die(sprintf("skip Failed to test for MySQL SSL support, [%d] %s", $link->errno, $link->error));
	}
}


if (empty($row))
	die(sprintf("skip Failed to test for MySQL SSL support, [%d] %s", $link->errno, $link->error));

if (($row[1] == 'NO') || ($row[1] == 'DISABLED'))
	die(sprintf("skip MySQL has no SSL support, [%d] %s", $link->errno, $link->error));

$link->close();
?>
--FILE--
<?php
	include "connect.inc";
	$db1 = new mysqli();


	$flags = MYSQLI_CLIENT_SSL | MYSQLI_CLIENT_SSL_DONT_VERIFY_SERVER_CERT;

	$link = mysqli_init();
	mysqli_ssl_set($link, null, null, null, null, "RC4-MD5");
	if (my_mysqli_real_connect($link, 'p:' . $host, $user, $passwd, $db, $port, null, $flags)) {
		$r = $link->query("SHOW STATUS LIKE 'Ssl_cipher'");
		var_dump($r->fetch_row());
	}

	/* non-persistent connection */
	$link2 = mysqli_init();
	mysqli_ssl_set($link2, null, null, null, null, "RC4-MD5");
	if (my_mysqli_real_connect($link2, $host, $user, $passwd, $db, $port, null, $flags)) {
		$r2 = $link2->query("SHOW STATUS LIKE 'Ssl_cipher'");
		var_dump($r2->fetch_row());
	}

	echo "done\n";
?>
--EXPECTF--
array(2) {
  [0]=>
  string(10) "Ssl_cipher"
  [1]=>
  string(7) "RC4-MD5"
}
array(2) {
  [0]=>
  string(10) "Ssl_cipher"
  [1]=>
  string(7) "RC4-MD5"
}
done