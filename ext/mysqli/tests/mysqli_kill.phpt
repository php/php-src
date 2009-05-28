--TEST--
mysqli_kill()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_kill()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_kill($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');

	// Zend will cast the NULL to 0
	if (!is_bool($tmp = mysqli_kill($link, null)))
		printf("[003] Expecting boolean/any, got %s/%s\n", gettype($tmp), $tmp);

	if (!$thread_id = mysqli_thread_id($link))
		printf("[004] Cannot determine thread id, [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	$tmp = mysqli_kill($link, $thread_id);
	if (!is_bool($tmp))
		printf("[005] Expecting boolean/any, got %s/%s\n", gettype($tmp), $tmp);

	if ($res = mysqli_query($link, "SELECT id FROM test LIMIT 1"))
		pintf("[006] Expecting boolean/false, got %s/%s\n", gettype($res), $res);

	var_dump($error = mysqli_error($link));
	if (!is_string($error) || ('' === $error))
		printf("[007] Expecting string/any non empty, got %s/%s\n", gettype($error), $error);
	var_dump($res);
	var_dump($link);

	mysqli_close($link);

	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[008] Cannot connect, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	mysqli_kill($link, -1);
	if ((!$res = mysqli_query($link, "SELECT id FROM test LIMIT 1")) ||
		(!$tmp = mysqli_fetch_assoc($res))) {
		printf("[009] Connection should not be gone, [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}
	var_dump($tmp);
	mysqli_free_result($res);
	mysqli_close($link);

	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[010] Cannot connect, [%d] %s\n", mysqli_connect_errno(), mysqli_connect_error());

	mysqli_change_user($link, "This might work if you accept anonymous users in your setup", "password", $db);      mysqli_kill($link, -1);

	mysqli_close($link);

	print "done!";
?>
--EXPECTF--
Warning: mysqli_kill(): processid should have positive value in %s on line %d
%unicode|string%(%d) "%s"
bool(false)
object(mysqli)#%d (%d) {
  [%u|b%"affected_rows"]=>
  int(-1)
  [%u|b%"client_info"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"client_version"]=>
  int(%d)
  [%u|b%"connect_errno"]=>
  int(0)
  [%u|b%"connect_error"]=>
  %unicode|string%(0) ""
  [%u|b%"errno"]=>
  int(2006)
  [%u|b%"error"]=>
  %unicode|string%(26) "MySQL server has gone away"
  [%u|b%"field_count"]=>
  int(0)
  [%u|b%"host_info"]=>
  %unicode|string%(42) "MySQL host info: Localhost via UNIX socket"
  [%u|b%"info"]=>
  %unicode|string%(38) "Records: 6  Duplicates: 0  Warnings: 0"
  [%u|b%"insert_id"]=>
  int(0)
  [%u|b%"server_info"]=>
  %unicode|string%(%d) "%s"
  [%u|b%"server_version"]=>
  int(%d)
  [%u|b%"sqlstate"]=>
  %unicode|string%(5) "HY000"
  [%u|b%"protocol_version"]=>
  int(10)
  [%u|b%"thread_id"]=>
  int(%d)
  [%u|b%"warning_count"]=>
  int(0)
}

Warning: mysqli_kill(): processid should have positive value in %s on line %d
array(1) {
  [%u|b%"id"]=>
  %unicode|string%(1) "1"
}

Warning: mysqli_kill(): processid should have positive value in %s on line %d
done!