--TEST--
mysqli_close()
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

	if (!is_null($tmp = @mysqli_close()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_close($link, $link)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket))
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);

	$tmp = @mysqli_close(NULL);
	if (NULL !== $tmp)
		printf("[004] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	$tmp = mysqli_close($link);
	if (true !== $tmp)
		printf("[005] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	if (false !== ($tmp = @mysqli_query($link, "SELECT 1")))
		printf("[006] Expecting false, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECT--
done!
