--TEST--
mysqli_dump_debug_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	$tmp	= NULL;
	$link	= NULL;

	if (NULL !== ($tmp = @mysqli_dump_debug_info()))
		printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_dump_debug_info($link)))
		printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = my_mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
		exit(1);
	}

	if (!is_bool($tmp = mysqli_dump_debug_info($link)))
		printf("[004] Expecting boolean/[true|false] value, got %s/%s, [%d] %s\n",
			gettype($tmp), $tmp,
			mysqli_errno($link), mysqli_error($link));

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_dump_debug_info($link)))
		printf("[005] Expecting NULL, got %s/%s\n",
			gettype($tmp), $tmp,
			mysqli_errno($link), mysqli_error($link));

	print "done!";
?>
--EXPECTF--
Warning: mysqli_dump_debug_info(): Couldn't fetch mysqli in %s on line %d
done!
