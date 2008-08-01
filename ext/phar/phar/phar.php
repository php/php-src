#!/usr/local/bin/php
<?php

/** @file phar.php
 * @ingroup Phar
 * @brief class CLICommand
 * @author  Marcus Boerger
 * @date    2007 - 2008
 *
 * Phar Command
 */

if (!extension_loaded('phar'))
{
	if (!class_exists('PHP_Archive', 0)) {
		echo "Neither Extension Phar nor class PHP_Archive are available.\n";
		exit(1);
	}
	if (!in_array('phar', stream_get_wrappers())) {
		stream_wrapper_register('phar', 'PHP_Archive');
	}
	if (!class_exists('Phar',0)) {
		require 'phar://'.__FILE__.'/phar.inc';
	}
}

foreach(array("SPL", "Reflection") as $ext)
{
	if (!extension_loaded($ext)) {
		echo "$argv[0] requires PHP extension $ext.\n";
		exit(1);
	}
}

function command_include($file)
{
	$file = 'phar://' . __FILE__ . '/' . $file;
	if (file_exists($file)) {
		include($file);
	}
}

function command_autoload($classname)
{
	command_include(strtolower($classname) . '.inc');
}

Phar::mapPhar();

spl_autoload_register('command_autoload');

new PharCommand($argc, $argv);

__HALT_COMPILER();

?>