--TEST--
mysqli_get_server_info()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	include "connect.inc";

	if (!is_null($tmp = @mysqli_get_server_info()))
		printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	if (!is_null($tmp = @mysqli_get_server_info(NULL)))
		printf("[002] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	require "table.inc";
	if (!is_string($info = mysqli_get_server_info($link)) || ('' === $info))
		printf("[003] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

	if (!is_null($tmp = @mysqli_get_server_info('too', 'many arguments')))
		printf("[005] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
done!