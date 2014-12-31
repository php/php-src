--TEST--
ISSUE #183 (TMP_VAR is not only used once)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (PHP_OS != "Linux") die("skip, only for linux"); ?>
--FILE--
<?php

switch (PHP_OS) {
	case "Windows":
	break;
	case "Darwin":
	break;
	case "Linux":
		echo "okey";
	break;
	default:
	break;
}
--EXPECT--
okey
