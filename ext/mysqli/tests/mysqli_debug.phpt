--TEST--
mysqli_debug()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifemb.inc');
if (!function_exists('mysqli_debug'))
 	die("skip: mysqli_debug() not available");
?>
--FILE--
<?php
	include "connect.inc";

	$tmp    = NULL;
	$link   = NULL;

	if (NULL !== ($tmp = @mysqli_debug()))
		printf("[001] Expecting NULL/NULL, got %s/%s\n", gettype($tmp), $tmp);

	// NOTE: documentation is not clear on this: function always return NULL or TRUE
	if (true !== ($tmp = mysqli_debug("d:t:O,/tmp/client.trace")))
		printf("[002] Expecting boolean/true, got %s/%s\n", gettype($tmp), $tmp);

	print "done!";
?>
--EXPECTF--
done!