--TEST--
readline_read_history(): Test that open_basedir is respected
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_read_history')) die("skip"); ?>
--INI--
open_basedir=/tmp/some-sandbox
--FILE--
<?php

$name = '/tmp/out-of-sandbox';

var_dump(readline_read_history($name));

?>
--EXPECTF--
Warning: readline_read_history(): open_basedir restriction in effect. File(/tmp/out-of-sandbox) is not within the allowed path(s): (/tmp/some-sandbox) in %s on line %d
bool(false)
