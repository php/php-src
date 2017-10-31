--TEST--
mysqli_fetch_lengths()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!$mysqli = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[001] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}

	if (!is_null($tmp = @mysqli_fetch_lengths()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_fetch_lengths($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require('table.inc');
	if (!$res = mysqli_query($link, "SELECT id, label FROM test ORDER BY id LIMIT 1")) {
		printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	var_dump(mysqli_fetch_lengths($res));
	while ($row = mysqli_fetch_assoc($res))
		var_dump(mysqli_fetch_lengths($res));
	var_dump(mysqli_fetch_lengths($res));

	mysqli_free_result($res);

	var_dump(mysqli_fetch_lengths($res));

	mysqli_close($link);
	print "done!";
?>
--CLEAN--
<?php
	require_once("clean_table.inc");
?>
--EXPECTF--
bool(false)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
bool(false)

Warning: mysqli_fetch_lengths(): Couldn't fetch mysqli_result in %s on line %d
bool(false)
done!
