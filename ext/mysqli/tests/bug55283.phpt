--TEST--
Bug #55283 (SSL options set by mysqli_ssl_set ignored for MySQLi persistent connections)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
$link = mysqli_init();
mysqli_ssl_set($link, null, null, null, null, "RC4-MD5");
if (my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, null, $flags)) {
	$res = $link->query("SHOW STATUS LIKE 'Ssl_cipher'");
	$row = $res->fetch_row();
	if ($row[1] === "") {
		die('skip Server started without SSL support');	
	}
}
?>
--FILE--
<?php
	include "connect.inc";
	$db1 = new mysqli();


	$flags = MYSQLI_CLIENT_SSL;
	
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