<?php

/** @file inigroups.inc
 * @ingroup Examples
 * @brief class IniGroups
 * @author  Marcus Boerger
 * @date    2003 - 2005
 *
 * SPL - Standard PHP Library
 */

if (!class_exists("KeyFilter", false)) require_once("keyfilter.inc");
if (!class_exists("DbaReader", false)) require_once("dbareader.inc");

/** @ingroup Examples
 * @brief   Class to iterate all groups within an ini file.
 * @author  Marcus Boerger
 * @version 1.1
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
		parent::__construct(new DbaReader($file, 'inifile'), '^\[.*\]$');
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