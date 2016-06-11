#!/usr/bin/env php
<?php

/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Bob Weinand <bwoebi@php.net>                                |
   +----------------------------------------------------------------------+
*/

#####
## This is just a helper for intercepting stdin/stdout and the file and create a half-finished test.
## The output still may need adapting to match file names etc.
#####

error_reporting(-1);

$phpdbg = getenv('TEST_PHPDBG_EXECUTABLE') ?: null;
$pass_options = ' -qbI -n -d "error_reporting=32767" -d "display_errors=1" -d "display_startup_errors=1" -d "log_errors=0"';
$file = "";
$cmdargv = "";

if (isset($argc) && $argc > 1) {
	$post_ddash = false;
	for ($i = 1; $i < $argc; $i++) {
		if ($argv[$i][0] == "-" && !$post_ddash) {
			switch (substr($argv[$i], 1)) {
				case "p":
					$phpdbg = $argv[++$i];
					break;
				case "n":
					$pass_options .= " -n";
					break;
				case "d":
					$pass_options .= " -d ".escapeshellarg($argv[++$i]);
					$ini[] = $argv[$i];
					break;
				case "-":
					$post_ddash = true;
					break;
			}
		} else {
			$real_argv[] = $argv[$i];
		}
	}
	if (isset($real_argv[0])) {
		$file = realpath($real_argv[0]);
		$cmdargv = implode(" ", array_map("escapeshellarg", array_slice($real_argv, 1)));
	}
}

$proc = proc_open("$phpdbg $pass_options $file -- $cmdargv", [["pipe", "r"], ["pipe", "w"], ["pipe", "w"]], $pipes);
if (!$proc) {
	die("Couldn't start phpdbg\n");
}

$input = $output = "";

stream_set_blocking(STDIN, false);

do {
	$r = [$pipes[1], STDIN];
	$w = $e = null;
	$n = @stream_select($r, $w, $e, null);

	if ($n > 0) {
		if ("" != $in = fread(STDIN, 1024)) {
			$input .= $in;
			fwrite($pipes[0], $in);
			continue;
		}

		if (feof(STDIN)) {
			die("stdin closed?!\n");
		}

		if (feof($pipes[1])) {
			$n = false;
		} else {
			$output .= $c = fgetc($pipes[1]);
			echo $c;
		}
	}
} while ($n !== false);

stream_set_blocking(STDIN, true);

print "\n";
if (!isset($name)) {
	print "Specify the test description: ";
	$desc = trim(fgets(STDIN));
}
while (!isset($testfile)) {
	print "Specify the test file name (leave empty to write to stderr): ";
	$testfile = trim(fgets(STDIN));
	if ($testfile != "" && file_exists($testfile)) {
		print "That file already exists. Type y or yes to overwrite: ";
		$y = trim(fgets(STDIN));
		if ($y !== "y" && $y !== "yes") {
			unset($testfile);
		}
	}
}

$output = str_replace("string(".strlen($file).") \"$file\"", 'string(%d) "%s"', $output);
$output = str_replace($file, "%s", $output);
$input = trim($input);

$testdata = <<<TEST
--TEST--
$desc
--PHPDBG--
$input
--EXPECTF--
$output
TEST;

if (!empty($ini)) {
	$testdata .= "\n--INI--\n".implode("\n", $ini);
}
if ($cmdargv != "") {
	$testdata .= "\n--ARGS--\n$cmdargv";
}
if ($file != "") {
	$testdata .= "\n--FILE--\n".file_get_contents($file);
}

if ($testfile == "") {
	print "\n";
} elseif (file_put_contents($testfile, $testdata)) {
	print "Test saved to $testfile\n";
} else {
	print "The test could not be saved to $testfile; outputting on stderr now\n";
	$testfile = "";
}

if ($testfile == "") {
	fwrite(STDERR, $testdata);
}
