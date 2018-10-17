--TEST--
mysqli_prepare() - no object on failure
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require('table.inc');

	if (false !== ($tmp = mysqli_prepare($link, false)))
		printf("[001] Expecting boolean/false, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
	printf("a) [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	if (false !== ($tmp = mysqli_prepare($link, '')))
		printf("[002] Expecting boolean/false, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
	printf("b) [%d] %s\n", mysqli_errno($link), mysqli_error($link));

	mysqli_close($link);

	if (!$mysqli = new my_mysqli($host, $user, $passwd, $db, $port, $socket))
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	if (false !== ($tmp = $mysqli->prepare(false)))
		printf("[004] Expecting boolean/false, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
	printf("c) [%d] %s\n", $mysqli->errno, $mysqli->error);

	if (false !== ($tmp = $mysqli->prepare('')))
		printf("[005] Expecting boolean/false, got %s/%s\n", gettype($tmp), (is_object($tmp) ? var_dump($tmp, true) : $tmp));
	printf("c) [%d] %s\n", $mysqli->errno, $mysqli->error);

	print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECT--
a) [1065] Query was empty
b) [1065] Query was empty
c) [1065] Query was empty
c) [1065] Query was empty
done!
