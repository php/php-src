<?php

/** List groups within an ini file
 *
 * Usage: php dba_dump.php <file> [<regex>]
 *
 * Show all groups in the ini file specified by <file>.
 * The regular expression <regex> is used to filter the result.
 *
 * Note: configure with --enable-dba 
 *
 * (c) Marcus Boerger, 2003 - 2004
 */

if ($argc < 2) {
	echo <<<EOF
Usage: php dba_dump.php <file> [<regex>]

Show all groups in the ini file specified by <file>.
The regular expression <regex> is used to filter the result.


EOF;
	exit(1);
}

require_once("dba_reader.inc");
require_once("IniGroups.inc");

$it = new IniGroups($argv[1]);
if ($argc>2) {
	$it = new KeyFilter($it, $argv[2]);
}

foreach($it as $group) {
	echo "$group\n";
}

?>