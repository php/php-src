<?php
/* $Id$ */

if (php_sapi_name() != "cli") {
	echo "Please run this script using the CLI version of PHP\n";
	exit;
}
/*
	This script can be used on Win32 systems
	
	1) Make sure you have CygWin installed
	2) Adjust the $cygwin_path to match your installation
	3) Change the environment cariable PATHEXT to include .PHP
	4) run ext_skel --ext_name=...
		the first time you run this script you will be asked to 
		associate it with a program. chooses the CLI version of php.
*/

$cygwin_path = 'c:\cygwin\bin';

$path = getenv("PATH");
putenv("PATH=$cygwin_path;$path");

array_shift($argv);
system("sh ext_skel " . implode(" ", $argv));

$extname = "";
$skel = "skeleton";
foreach($argv as $arg) {
	if (strtolower(substr($arg, 0, 9)) == "--extname") {
		$extname = substr($arg, 10);
	}
	if (strtolower(substr($arg, 0, 6)) == "--skel") {
		$skel = substr($arg, 7);
	}
}

$fp = fopen("$skel/skeleton.dsp", "rb");
if ($fp) {
	$dsp_file = fread($fp, filesize("$skel/skeleton.dsp"));
	fclose($fp);
	
	$dsp_file = str_replace("extname", $extname, $dsp_file);
	$dsp_file = str_replace("EXTNAME", strtoupper($extname), $dsp_file);
	$fp = fopen("$extname/$extname.dsp", "wb");
	if ($fp) {
		fwrite($fp, $dsp_file);
		fclose($fp);
	}
}

?>