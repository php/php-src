--TEST--
Bug #71273 A wrong ext directory setup in php.ini leads to crash
--SKIPIF--
<?php
	if ("cli" != php_sapi_name()) {
		die("skip CLI only");
	}
?>
--FILE--
<?php
	/* NOTE this file is required to be encoded in iso-8859-1 */

	$cmd = getenv('TEST_PHP_EXECUTABLE') . " -n -d html_errors=on -d extension_dir=a/é/w -d extension=php_kartoffelbrei.dll -v 2>&1";
	$out = shell_exec($cmd);

	var_dump(preg_match(",.+a[\\/].+[\\/]w.php_kartoffelbrei.dll.+,s", $out));
?>
==DONE==
--EXPECTF--
int(1)
==DONE==
