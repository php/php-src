--TEST--
Bug #73462 (Persistent connections don't set $connect_errno)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$mysql_1 = new mysqli('p:'.$host, $user, $passwd, $db);
	$mysql_1->close();

	$mysql_2 = @new mysqli('DOESNT-EXIST', $user, $passwd, $db);
	@$mysql_2->close();

	$mysql_3 = new mysqli('p:'.$host, $user, $passwd, $db);
	$error = mysqli_connect_errno();
	$mysql_3->close();

	if ($error !== 0) printf("[001] Expecting '0' got '%d'.\n", $error);

	print "done!";
?>
--EXPECTF--
done!
