--TEST--
Bug #51647 (Certificate file without private key (pk in another file) doesn't work)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include ("connect.inc");

	$link = mysqli_init();
	$link->ssl_set("client-key.pem", "client-cert.pem", "cacert.pem","","");
	if (!my_mysqli_real_connect($link, $host, $user, $passwd, $db, $port, $socket)) {
		printf("[002] Connect failed, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());
	}
	var_dump($link->query("show status like \"Ssl_cipher\"")->fetch_assoc());

	print "done!";
?>
--EXPECTF--
array(2) {
  ["Variable_name"]=>
  string(10) "Ssl_cipher"
  ["Value"]=>
  string(%d) "%S"
}
done!
