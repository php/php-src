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
 * (c) Marcus Boerger, 2003
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
require_once("key_filter.inc");

/**
 * @brief   Class to iterate all groups within an ini file.
 * @author  Marcus Boerger
 * @version 1.0
 *
 * Using this class you can iterator over all groups of a ini file.
 * 
 * This class uses a 'is-a' relation to key_filter in contrast to a 'has-a'
 * relation. Doing so both current() and key() methods must be overwritten. 
 * If it would use a 'has-a' relation there would be much more to type...
 * but for puritists that would allow correctness in so far as then no 
 * key() would be needed.
 */
class ini_groups extends key_filter
{
	/**
	 * Construct an ini file group iterator from a filename.
	 *
	 * @param file Ini file to open.
	 */
	function __construct($file) {
		parent::__construct(new dba_reader($file, 'inifile'), '^\[.*\]$');
	}

	/**
	 * @return The current group.
	 */
	function current() {
		return substr(parent::key(),1,-1);
	}

	/**
	 * @return The current group.
	 */
	function key() {
		return substr(parent::key(),1,-1);
	}
}

$it = new ini_groups($argv[1]);
if ($argc>2) {
	$it = new key_filter($it, $argv[2]);
}

foreach($it as $group) {
	echo "$group\n";
}

?>