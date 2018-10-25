--TEST--
gd_info()
--SKIPIF--
<?php
	if (!function_exists('gd_info')) {
		die('skip gd_info() not available');
	}
?>
--FILE--
<?php

/* Prototype  : array gd_info  ( void  )
 * Description: Retrieve information about the currently installed GD library
 * Source code: ext/standard/image.c
 * Alias to functions:
 */
	echo "basic test of gd_info() function\n";

	var_dump(gd_info());

	echo "\nDone\n";
?>
--EXPECTF--
basic test of gd_info() function
array(%d) {
%a
}

Done
