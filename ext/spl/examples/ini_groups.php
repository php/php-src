<?php

/** @file   ini_groups.php
 * @brief   Program List groups within an ini file
 * @ingroup Examples
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * Usage: php dba_dump.php \<file\> [\<regex\>]
 *
 * Show all groups in the ini file specified by \<file\>.
 * The regular expression \<regex\> is used to filter the result.
 *
 * Note: configure with --enable-dba 
 */

if ($argc < 2) {
	echo <<<EOF
Usage: php dba_dump.php <file> [<regex>]

Show all groups in the ini file specified by <file>.
The regular expression <regex> is used to filter the result.


EOF;
	exit(1);
}

if (!class_exists("KeyFilter", false)) require_once("keyfilter.inc");
if (!class_exists("IniGroups", false)) require_once("inigroups.inc");

$it = new IniGroups($argv[1]);
if ($argc>2) {
	$it = new KeyFilter($it, $argv[2]);
}

foreach($it as $group) {
	echo "$group\n";
}

?>