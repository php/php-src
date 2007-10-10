--TEST--
mysqli_rpl_probe()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
if (!function_exists('mysqli_rpl_probe')) {
	die("skip mysqli_rpl_probe() not available");
}
require_once('connect.inc');
if (!$TEST_EXPERIMENTAL)
	die("skip - experimental (= unsupported) feature");
?>
--FILE--
<?php
	/* NOTE: tests is a stub, but function is deprecated, as long as it does not crash when invoking it... */
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	if (NULL !== ($tmp = @mysqli_rpl_probe()))
		printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (NULL !== ($tmp = @mysqli_rpl_probe($link)))
		printf("[002] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!$link = mysqli_connect($host, $user, $passwd, $db, $port, $socket)) {
		printf("[003] Cannot connect to the server using host=%s, user=%s, passwd=***, dbname=%s, port=%s, socket=%s\n",
			$host, $user, $db, $port, $socket);
	}

	if (!is_bool($tmp = mysqli_rpl_probe($link)))
		printf("[004] Expecting boolean/any value, got %s/%s\n", gettype($tmp), $tmp);

	mysqli_close($link);

	if (NULL !== ($tmp = mysqli_rpl_probe($link)))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
Warning: mysqli_rpl_probe(): Couldn't fetch mysqli in %s on line %d
done!