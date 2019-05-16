--TEST--
PAM auth plugin
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');

if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
	die(sprintf("SKIP Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
		$host, $user, $db, $port, $socket));
}

if ($link->server_version < 50500)
	die(sprintf("SKIP Needs MySQL 5.5 or newer, found MySQL %s\n", $link->server_info));

if (!$res = $link->query("SHOW PLUGINS"))
	die(sprintf("SKIP [%d] %s\n", $link->errno, $link->error));

$have_pam = false;
while ($row = $res->fetch_assoc()) {
	if (isset($row['Name']) && ('mysql_clear_password' == $row['Name'])) {
		$have_pam = true;
		break;
	}
}
$res->close();

if (!$have_pam)
  die("SKIP Server PAM plugin not installed");


mysqli_query($link, 'DROP USER pamtest');
mysqli_query($link, 'DROP USER pamtest@localhost');

if (!mysqli_query($link, 'CREATE USER pamtest@"%" IDENTIFIED WITH mysql_clear_password') ||
	!mysqli_query($link, 'CREATE USER pamtest@"localhost" IDENTIFIED WITH mysql_clear_password')) {
	printf("skip Cannot create second DB user [%d] %s", mysqli_errno($link), mysqli_error($link));
	mysqli_close($link);
	die("skip CREATE USER failed");
}

if (!$link->query("CREATE TABLE test (id INT)") || !$link->query("INSERT INTO test(id) VALUES (1)"))
	die(sprintf("SKIP [%d] %s\n", $link->errno, $link->error));



if (!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pamtest@'%%'", $db)) ||
	!mysqli_query($link, sprintf("GRANT SELECT ON TABLE %s.test TO pamtest@'localhost'", $db))) {
	printf("skip Cannot grant SELECT to user [%d] %s", mysqli_errno($link), mysqli_error($link));
	mysqli_close($link);
	die("skip GRANT failed");
}
?>
--INI--
max_execution_time=240
--FILE--
<?php
	require_once('connect.inc');
	require_once('table.inc');

	if (!$link = my_mysqli_connect($host, 'pamtest', 'pamtest', $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	} else {

	  if (!$res = $link->query("SELECT id FROM test WHERE id = 1"))
		  printf("[002] [%d] %s\n", $link->errno, $link->error);

	  if (!$row = mysqli_fetch_assoc($res)) {
		  printf("[003] [%d] %s\n", $link->errno, $link->error);
	  }

	  if ($row['id'] != 1) {
		  printf("[004] Expecting 1 got %s/'%s'", gettype($row['id']), $row['id']);
	  }

	  $res->close();
	  $link->close();
	}

	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
	mysqli_query($link, 'DROP USER pamtest');
	mysqli_query($link, 'DROP USER pamtest@localhost');
?>
--EXPECTF--
Warning: mysqli_real_connect(): (28000/1045): Access denied for user %s
[001] Cannot connect to the server using host=%s
done!
