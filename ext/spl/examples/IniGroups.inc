<?php

require_once("KeyFilter.inc");

/**
 * @brief   Class to iterate all groups within an ini file.
 * @author  Marcus Boerger
 * @version 1.0
 *
 * Using this class you can iterator over all groups of a ini file.
 * 
 * This class uses a 'is-a' relation to KeyFilter in contrast to a 'has-a'
 * relation. Doing so both current() and key() methods must be overwritten. 
 * If it would use a 'has-a' relation there would be much more to type...
 * but for puritists that would allow correctness in so far as then no 
 * key() would be needed.
 */
class IniGroups extends KeyFilter
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

?>