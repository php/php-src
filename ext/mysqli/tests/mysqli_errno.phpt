--TEST--
mysqli_errno()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp    = NULL;
	$link   = NULL;

	if (!is_null($tmp = @mysqli_errno()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_errno($link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
}

	var_dump(mysqli_errno($link));

	if (!mysqli_query($link, 'DROP TABLE IF EXISTS test')) {
		printf("[004] Failed to drop old test table: [%d] %s\n", mysqli_errno($link), mysqli_error($link));
	}

	mysqli_query($link, 'SELECT * FROM test');
	var_dump(mysqli_errno($link));

	@mysqli_query($link, 'No SQL');
	if (($tmp = mysqli_errno($link)) == 0)
		printf("[005] Expecting int/any non zero got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	var_dump(mysqli_errno($link));

	print "done!";
?>
--EXPECTF--
int(0)
int(%d)

Warning: mysqli_errno(): Couldn't fetch mysqli in %s on line %d
NULL
done!
