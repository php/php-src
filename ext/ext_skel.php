#!/usr/bin/env php
<?php
/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Kalle Sommer Nielsen <kalle@php.net>                        |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* {{{ error
 */
function error($message) {
	printf('Error: %s%s', $message, PHP_EOL);
	exit;
}
/* }}} */

/* {{{ print_help
 */
function print_help() {
	printf('php ext_skel.php --ext <name> [--experimental] [--author <name>]%s', PHP_EOL);
	printf('                 [--dir <path>] [--std] [--onlyunix]%s', PHP_EOL);
	printf('                 [--onlywindows] [--help]%1$s%1$s', PHP_EOL);
	printf('  --ext <name>		The name of the extension defined as <name>%s', PHP_EOL);
	printf('  --experimental	Passed if this extension is experimental, this creates%s', PHP_EOL);
	printf('                        the EXPERIMENTAL file in the root of the extension%s', PHP_EOL);
	printf('  --author <name>       Your name, this is used if --std is passed and%s', PHP_EOL);
	printf('                        for the CREDITS file%s', PHP_EOL);
	printf('  --dir <path>		Path to the directory for where extension should be%s', PHP_EOL);
	printf('                        created. Defaults to the directory of where this script%s', PHP_EOL);
	printf(' 			lives%s', PHP_EOL);
	printf('  --std			If passed, the standard header and vim rules footer used%s', PHP_EOL);
	printf(' 			in extensions that is included in the core, will be used%s', PHP_EOL);
	printf('  --onlyunix		Only generate configure scripts for Unix%s', PHP_EOL);
	printf('  --onlywindows		Only generate configure scripts for Windows%s', PHP_EOL);
	printf('  --help                This help%s', PHP_EOL);

	exit;
}
/* }}} */

/* {{{ task
 */
function task($label, $callback) {
	printf('%s... ', $label);

	$callback();

	printf('done%s', PHP_EOL);
}
/* }}} */

/* {{{ print_success
 */
function print_success() {
	global $options;

	if (PHP_OS_FAMILY != 'Windows') {
		$file_prefix = './';
		$make_prefix = '';
	} else {
		$file_prefix = '';
		$make_prefix = 'n';
	}

	printf('%1$sSuccess. The extension is now ready to be compiled into PHP. To do so, use the%s', PHP_EOL);
	printf('following steps:%1$s%1$s', PHP_EOL);
	printf('cd /path/to/php-src%s', PHP_EOL);
	printf('%sbuildconf%s', $file_prefix, PHP_EOL);
	printf('%sconfigure --enable-%s%s', $file_prefix, $options['ext'], PHP_EOL);
	printf('%smake%2$s%2$s', $make_prefix, PHP_EOL);
	printf('Don\'t forget to run tests once the compilation is done:%s', PHP_EOL);
	printf('%smake test TESTS=ext/%s/tests%3$s%3$s', $make_prefix, $options['ext'], PHP_EOL);
	printf('Thank you for using PHP!%s', PHP_EOL);
}
/* }}} */

/* {{{ process_args
 */
function process_args($argv, $argc) {
	$options = [
			'unix'		=> true,
			'windows' 	=> true,
			'ext' 		=> '',
			'dir'		=> __DIR__ . DIRECTORY_SEPARATOR,
			'skel' 		=> __DIR__ . DIRECTORY_SEPARATOR . 'skeleton' . DIRECTORY_SEPARATOR,
			'author'	=> false,
			'experimental'	=> false,
			'std'		=> false
			];

	for($i = 1; $i < $argc; ++$i)
	{
		$val = $argv[$i];

		if($val{0} != '-' || $val{1} != '-')
		{
			continue;
		}

		switch($opt = strtolower(substr($val, 2)))
		{
			case 'help': {
				print_help();
			}
			case 'onlyunix': {
				$options['windows'] = false;
			}
			break;
			case 'onlywindows': {
				$options['unix'] = false;
			}
			break;
			case 'experimental': {
				$options['experimental'] = true;
			}
			break;
			case 'std': {
				$options['std'] = true;
			}
			break;
			case 'ext':
			case 'dir':
			case 'author': {
				if (!isset($argv[$i + 1]) || ($argv[$i + 1]{0} == '-' && $argv[$i + 1]{1} == '-')) {
					error('Argument "' . $val . '" expects a value, none passed');
				} else if ($opt == 'dir' && empty($argv[$i + 1])) {
					continue 2;
				}

				$options[$opt] = ($opt == 'dir' ? realpath($argv[$i + 1]) . DIRECTORY_SEPARATOR : $argv[$i + 1]);
			}
			break;
			default: {
				error('Unsupported argument "' . $val . '" passed');
			}
		}
	}

	if (empty($options['ext'])) {
		error('No extension name passed, use "--ext <name>"');
	} else if (!$options['unix'] && !$options['windows']) {
		error('Cannot pass both --onlyunix and --onlywindows');
	} else if (!is_dir($options['skel'])) {
		error('The skeleton directory was not found');
	}

	$options['ext'] = str_replace(['\\', '/'], '', strtolower($options['ext']));

	return $options;
}
/* }}} */

/* {{{ process_source_tags
 */
function process_source_tags($file, $short_name) {
	global $options;

	$source = file_get_contents($file);

	if ($source === false) {
		error('Unable to open file for reading: ' . $short_name);
	}

	$source = str_replace('%EXTNAME%', $options['ext'], $source);
	$source = str_replace('%EXTNAMECAPS%', strtoupper($options['ext']), $source);

	if (strpos($short_name, '.c') !== false || strpos($short_name, '.h') !== false) {
		static $header, $footer;

		if (!$header) {
			if ($options['std']) {
				$year = date('Y');
				$author_len = strlen($options['author']);
				$credits = $options['author'] . ($author_len && $author_len <= 60 ? str_repeat(' ', 60 - $author_len) : '');

				$header = <<<"HEADER"
/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-$year The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: $credits |
   +----------------------------------------------------------------------+
*/
HEADER;
				$footer = <<<'FOOTER'

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
FOOTER;
			} else {
				if ($options['author']) {
					$header = sprintf('/* %s extension for PHP (c) %d %s */', $options['ext'], date('Y'), $options['author']);
				} else {
					$header = sprintf('/* %s extension for PHP */', $options['ext']);
				}

				$footer = '';
			}
		}

		$source = str_replace(['%HEADER%', '%FOOTER%'], [$header, $footer], $source);
	}

	if (!file_put_contents($file, $source)) {
		error('Unable to save contents to file: ' . $short_name);
	}
}
/* }}} */

/* {{{ copy_config_scripts
 */
function copy_config_scripts() {
	global $options;

	$files = [];

	if ($options['unix']) {
		$files[] = 'config.m4';
	}

	if ($options['windows']) {
		$files[] = 'config.w32';
	}

	$files[] = '.gitignore';

	foreach($files as $config_script) {
		$new_config_script = $options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . $config_script;

		if (!copy($options['skel'] . $config_script . '.in', $new_config_script)) {
			error('Unable to copy config script: ' . $config_script);
		}

		process_source_tags($new_config_script, $config_script);
	}
}
/* }}} */

/* {{{ copy_sources
 */
function copy_sources() {
	global $options;

	$files = [
			'skeleton.c'		=> $options['ext'] . '.c',
			'php_skeleton.h'	=> 'php_' . $options['ext'] . '.h'
			];

	foreach ($files as $src_file => $dst_file) {
		if (!copy($options['skel'] . $src_file, $options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . $dst_file)) {
			error('Unable to copy source file: ' . $src_file);
		}

		process_source_tags($options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . $dst_file, $dst_file);
	}
}
/* }}} */

/* {{{ copy_tests
 */
function copy_tests() {
	global $options;

	$test_files = glob($options['skel'] . 'tests/*', GLOB_MARK);

	if (!$test_files) {
		return;
	}

	foreach ($test_files as $test) {
		if (is_dir($test)) {
			continue;
		}

		$new_test = str_replace([$options['skel'], '/'], ['', DIRECTORY_SEPARATOR], $test);

		if (!copy($test, $options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . $new_test)) {
			error('Unable to copy file: ' . $new_test);
		}

		process_source_tags($options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . $new_test, $new_test);
	}
}
/* }}} */


if (PHP_SAPI != 'cli') {
	error('This script is only suited for CLI');
}

if ($argc < 1) {
	print_help();
	exit;
}

$options = process_args($argv, $argc);

if (!$options['dir'] || !is_dir($options['dir'])) {
	error('The selected output directory does not exist');
} else if (is_dir($options['dir'] . $options['ext'])) {
	error('There is already a folder named "' . $options['ext'] . '" in the output directory');
} else if (!mkdir($options['dir'] . $options['ext'])) {
	error('Unable to create extension directory in the output directory');
} else if (!mkdir($options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . 'tests')) {
	error('Unable to create the tests directory');
}

if ($options['experimental']) {
	print('Creating EXPERIMENTAL... ');

	if (file_put_contents($options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . 'EXPERIMENTAL', '') === false) {
		error('Unable to create the EXPERIMENTAL file');
	}

	printf('done%s', PHP_EOL);
}

if (!empty($options['author'])) {
	print('Creating CREDITS... ');

	if (!file_put_contents($options['dir'] . $options['ext'] . DIRECTORY_SEPARATOR . 'CREDITS', $options['ext'] . PHP_EOL . $options['author'])) {
		error('Unable to create the CREDITS file');
	}

	printf('done%s', PHP_EOL);
}

date_default_timezone_set('UTC');

task('Copying config scripts', 'copy_config_scripts');
task('Copying sources', 'copy_sources');
task('Copying tests', 'copy_tests');

print_success();
