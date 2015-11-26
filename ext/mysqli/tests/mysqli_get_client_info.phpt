--TEST--
mysqli_get_client_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
	require_once("connect.inc");
	if (!is_string($info = mysqli_get_client_info()) || ('' === $info))
		printf("[001] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

	print "done!";
?>
--EXPECTF--
done!