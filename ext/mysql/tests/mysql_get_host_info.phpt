--TEST--
mysql_get_host_info()
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
include_once "connect.inc";

if (NULL !== ($tmp = @mysql_get_host_info(NULL)))
	printf("[001] Expecting NULL, got %s/%s\n", gettype($tmp), $tmp);

require "table.inc";
if (!is_string($info = mysql_get_host_info($link)) || ('' === $info))
	printf("[003] Expecting string/any_non_empty, got %s/%s\n", gettype($info), $info);

$def_info = mysql_get_host_info();
if ($def_info !== $info) {
	printf("[004] Host info for the default link and the specified link differ, [%d] %s\n",
		mysql_errno(), mysql_error());

	var_dump($def_info);
	var_dump($info);
}

if ((version_compare(PHP_VERSION, '5.9.9', '>') == 1) && !is_unicode($info)) {
	printf("[005] Expecting Unicode error message!\n");
	var_inspect($info);
}

if (!is_null($tmp = @mysql_get_host_info($link, "too many arguments"))) {
	printf("[006] Expecting NULL/NULL got %s/%s\n", $tmp, gettype($tmp));
}

print "done!";
?>
--CLEAN--
<?php
require_once("clean_table.inc");
?>
--EXPECTF--
done!
