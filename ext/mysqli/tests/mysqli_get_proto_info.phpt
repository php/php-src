--TEST--
mysqli_get_proto_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");

	if (!is_null($tmp = @mysqli_get_proto_info()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_get_proto_info(NULL)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require "table.inc";
	if (!is_int($info = mysqli_get_proto_info($link)) || ($info < 1))
		printf("[003] Expecting int/any_non_empty, got %s/%s\n", gettype($info), $info);

	if (!is_null($tmp = @mysqli_get_proto_info('too many', 'arguments')))
		printf("[004] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
done!
