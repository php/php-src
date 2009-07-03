--TEST--
mysql_get_client_info()
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
include "connect.inc";
if (!is_string($info = mysql_get_client_info()) || ('' === $info))
	printf("[001] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($info)) {
	printf("[002] Expecting Unicode!\n");
	var_inspect($info);
}

if (!is_null($tmp = @mysql_get_client_info("too many arguments"))) {
	printf("[003] Expecting NULL/NULL got %s/%s\n", $tmp, gettype($tmp));
}

print "done!";
?>
--EXPECTF--
done!