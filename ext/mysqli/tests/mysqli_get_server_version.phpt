--TEST--
mysqli_get_server_version()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!is_null($tmp = @mysqli_get_server_version()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_get_server_version(NULL)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require "table.inc";
	/* 5.1.5 -> 50105 -- major_version*10000 + minor_version *100 + sub_version */
	/* < 30000 = pre 3.2.3, very unlikely! */
	if (!is_int($info = mysqli_get_server_version($link)) || ($info < (3 * 10000)))
		printf("[003] Expecting int/any >= 30000, got %s/%s\n", gettype($info), $info);

	print "done!";
?>
--EXPECTF--
done!
