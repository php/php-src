--TEST--
Bug #74547 mysqli::change_user() doesn't accept null as $database argument w/strict_types
--SKIPIF--
<?php
  require_once('skipif.inc');
  require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	declare(strict_types=1);

	require_once("connect.inc");

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	var_dump(mysqli_change_user($link, $user, $passwd, NULL));

	mysqli_close($link);
?>
===DONE===
--EXPECT--
bool(true)
===DONE===
