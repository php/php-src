<?php

/* dba dump utility
 *
 * Usage: php dba_dump <file> <handler> [<regex>]
 *
 * Show all groups in the ini file specified by <file>.
 * The regular expression <regex> is used to filter the by setting name.
 *
 * Note: configure with --enable-dba 
 *
 * (c) Marcus Boerger
 */

require_once("dba_reader.inc");
require_once("key_filter.inc");

$db = new DbaReader($argv[1], $argv[2]);

if ($argc>3) {
	$db = new keyFilter($db, $argv[3]);
}

foreach($db as $key => $val) {
	echo "'$key' => '$val'\n";
}

?>