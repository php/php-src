--TEST--
mysqli_embedded_connect()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifnotemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	$tmp    = NULL;
	$link   = NULL;

	if (NULL !== ($tmp = @mysqli_embedded_connect()))
		printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = mysqli_embedded_connect($db)) {
		printf("[002] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}

	if (!is_bool($tmp = mysqli_embedded_connect($db . '_unknown')))
		printf("[003] Expecting boolean/[true|false] value, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	print "done!";
?>
--EXPECTF--
Warning: mysqli_embedded_connect() expects parameter 1 to be mysqli, null given in %s on line %d
done!